#include <DOM/JSAPI_DOMDocument.h>
#include "DatabaseSync.h"
#include "TransactionSync.h"
#include "../DatabaseException.h"
#include "../../Implementation/Database.h"
#include "../../Implementation/Data.h"
#include "../../Support/Convert.h"

using ::std::auto_ptr;
using ::std::back_insert_iterator;
using ::boost::optional;
using ::std::string;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

using Implementation::TransactionContext;
using Implementation::Data;

DatabaseSync::DatabaseSync(FB::BrowserHost host, const string& name, const string& description, const bool modifyDatabase)
	: Database(name, description),
	  host(host), 
	  modifyDatabase(modifyDatabase),
	  implementation(Implementation::AbstractDatabaseFactory::getInstance()
		.createDatabase(getOrigin(), name, description, modifyDatabase)),
	  metadata(implementation->getMetadata(), Metadata::Database, name),
	  #pragma warning(push)
	  #pragma warning(disable: 4355)
		transactionFactory(*this, *implementation)
	  #pragma warning(pop)
	{
	registerMethod("createObjectStore", make_method(this, static_cast<FB::JSOutObject (DatabaseSync::*)(const string&, const FB::CatchAll &)>(&DatabaseSync::createObjectStore))); 
	registerMethod("openObjectStore", make_method(this, static_cast<FB::JSOutObject (DatabaseSync::*)(const string&, const FB::CatchAll &)>(&DatabaseSync::openObjectStore))); 
	registerMethod("removeObjectStore", FB::make_method(this, &DatabaseSync::removeObjectStore));
	registerMethod("setVersion", FB::make_method(this, &DatabaseSync::setVersion));
	registerMethod("transaction", FB::make_method(this, static_cast<FB::JSOutObject (DatabaseSync::*)(const FB::CatchAll &)>(&DatabaseSync::transaction))); 
	}

DatabaseSync::~DatabaseSync()
	{
	if(getCurrentTransaction().is_initialized())
		getCurrentTransaction()->close();

	openObjectStores.release();
	}

//TODO: can KeyPath be null?  Add unit test; I think this is broken
FB::JSOutObject DatabaseSync::createObjectStore(const string& name, const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(values.size() < 1 || values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<bool>())
		throw FB::invalid_arguments();
	// If keyPath specified, it must be a string of length > 0
	else if(!values[0].empty() && (!values[0].is_of_type<string>() || values[0].cast<string>().length() == 0))
		throw FB::invalid_arguments();
	
	FB::variant keyPath = values[0];
	bool autoIncrement = values.size() == 2 ? values[1].cast<bool>() : true;

	return keyPath.empty() 
		? static_cast<FB::JSOutObject>(createObjectStore(name, autoIncrement))
		: static_cast<FB::JSOutObject>(createObjectStore(name, keyPath.cast<string>(), autoIncrement));
	}

FB::JSOutObject DatabaseSync::openObjectStore(const string& name, const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(values.size() > 1)
		throw FB::invalid_arguments();
	else if(values.size() == 1 && !values[0].is_of_type<int>())
		throw FB::invalid_arguments();

	Implementation::ObjectStore::Mode mode;
	try
		{ mode = values.size() == 1 
			? static_cast<Implementation::ObjectStore::Mode>(values[0].cast<int>())
			: Implementation::ObjectStore::READ_WRITE; }
	catch(std::bad_cast&)
		{ throw FB::invalid_arguments(); }

	return static_cast<FB::JSOutObject>(openObjectStore(name, mode));
	}

FB::variant DatabaseSync::getVersion() const
	{ return Convert::toVariant(host, metadata.getMetadata("version", transactionFactory.getTransactionContext())); }

long DatabaseSync::setVersion(const string& version)
	{
	metadata.putMetadata("version", Data(version), transactionFactory.getTransactionContext());

	//TODO 1.0 supports void return
	return 0;
	}

FB::AutoPtr<ObjectStoreSync> DatabaseSync::createObjectStore(const string& name, const string& keyPath, bool autoIncrement)
	{ 
	ensureCanCreateObjectStore(name);

	metadata.addToMetadataCollection("objectStores", name, transactionFactory, transactionFactory.getTransactionContext());
	FB::AutoPtr<ObjectStoreSync> objectStore = new ObjectStoreSync(host, *this, transactionFactory, transactionFactory.getTransactionContext(), metadata, name, keyPath, autoIncrement);
	openObjectStores.add(objectStore);

	return objectStore;
	}

FB::AutoPtr<ObjectStoreSync> DatabaseSync::createObjectStore(const string& name, bool autoIncrement)
	{ 
	ensureCanCreateObjectStore(name);

	try
		{
		auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();
		
		metadata.addToMetadataCollection("objectStores", name, transactionFactory, *transaction);
		FB::AutoPtr<ObjectStoreSync> objectStore = new ObjectStoreSync(host, *this, transactionFactory, *transaction, metadata, name, autoIncrement); 
		openObjectStores.add(objectStore);

		transaction->commit();

		return objectStore;
		}
	catch(Implementation::ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void DatabaseSync::ensureCanCreateObjectStore(const string& name)
	{
	StringVector objectStoreNames = this->getObjectStoreNames();
	
	if(!modifyDatabase)
		throw DatabaseException("CONSTRAINT_ERR", DatabaseException::CONSTRAINT_ERR);
	else if(std::find(objectStoreNames.begin(), objectStoreNames.end(), name) != objectStoreNames.end())
		throw DatabaseException("CONSTRAINT_ERR", DatabaseException::CONSTRAINT_ERR);
	}

FB::AutoPtr<ObjectStoreSync> DatabaseSync::openObjectStore(const string& name, Implementation::ObjectStore::Mode mode)
	{  
	if(mode != Implementation::ObjectStore::READ_ONLY 
	   && mode != Implementation::ObjectStore::READ_WRITE 
	   && mode != Implementation::ObjectStore::SNAPSHOT_READ)
		throw FB::invalid_arguments();

	try
		{
		FB::AutoPtr<ObjectStoreSync> objectStore = new ObjectStoreSync(host, *this, transactionFactory, transactionFactory.getTransactionContext(), metadata, name, mode);
		openObjectStores.add(objectStore);
		return objectStore;
		}
	catch(Implementation::ImplementationException& e)
		{ throw DatabaseException(e); }
	}

long DatabaseSync::removeObjectStore(const string& storeName)
	{ 
	StringVector objectStoreNames = getObjectStoreNames();

	if(find(objectStoreNames.begin(), objectStoreNames.end(), storeName) == objectStoreNames.end())
		throw DatabaseException("NOT_FOUND_ERR", DatabaseException::NOT_FOUND_ERR);
	
	//auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();

	openObjectStores.remove(storeName);
	metadata.removeFromMetadataCollection("objectStores", storeName, transactionFactory, transactionFactory.getTransactionContext());
	implementation->removeObjectStore(storeName, transactionFactory.getTransactionContext());

	//transaction->commit();

	return 0;
	}

StringVector DatabaseSync::getIndexNames()
	{
	StringVector indexes;
	back_insert_iterator<StringVector> insertIterator(indexes);
	StringVector objectStoreNames = metadata.getMetadataCollection("objectStores", transactionFactory.getTransactionContext());

	for(StringVector::iterator iterator = objectStoreNames.begin(); 
		iterator < objectStoreNames.end(); 
		++iterator)
		{
		StringVector indexNames(openObjectStore(*iterator, Implementation::ObjectStore::READ_ONLY)->getIndexNames());
		copy(indexNames.begin(), indexNames.end(), insertIterator);
		}

	return indexes;
	}

FB::AutoPtr<TransactionSync> DatabaseSync::transaction(const string& objectStoreName, const optional<unsigned int>& timeout)
	{ return transaction(optional<StringVector>(StringVector(1, objectStoreName)), timeout); }

FB::AutoPtr<TransactionSync> DatabaseSync::transaction(const FB::JSArray& objectStoreNames, const optional<unsigned int>& timeout)
	{ return transaction(optional<StringVector>(FB::convert_variant_list<StringVector>(objectStoreNames.Values())), timeout);  }

FB::JSOutObject DatabaseSync::transaction(const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<int>())
		throw FB::invalid_arguments();

	optional<unsigned int> timeout = values.size() == 2 
		? values[1].cast<int>() 
		: optional<unsigned int>();

	// We allow a single string, an array, or nothing as possible object store names
	// (Believe spec disallows a single string, but that's silly)
	if(values.size() == 0)
		transaction(optional<StringVector>(), timeout);
	else if(values[0].is_of_type<string>())
		transaction(values[0].cast<string>(), timeout);
	else if(values[0].is_of_type<FB::JSObject>())
		try
			{ transaction(FB::JSArray(values[0].cast<FB::JSObject>()), timeout); }
		catch(FB::bad_variant_cast)
			{ throw FB::invalid_arguments(); }
	else
		throw FB::invalid_arguments();
	
	if(!currentTransaction.is_initialized())
		throw DatabaseException("UNKNOWN_ERR", DatabaseException::UNKNOWN_ERR );
	else
		return static_cast<FB::JSOutObject>(currentTransaction.get());
	}

FB::AutoPtr<TransactionSync> DatabaseSync::transaction(optional<StringVector>& storeNames, const optional<unsigned int>& timeout)
	{
	if(currentTransaction.is_initialized())
		throw DatabaseException("NOT_ALLOWED_ERR", DatabaseException::NOT_ALLOWED_ERR);
	else if(storeNames.is_initialized())
		{
		StringVector objectStoreNames = getObjectStoreNames();
		sort(storeNames->begin(), storeNames->end());
		sort(objectStoreNames.begin(), objectStoreNames.end());

		// Ensure that the passed-in object store names actually exist in our database
		if(!std::includes(storeNames->begin(), storeNames->end(), objectStoreNames.begin(), objectStoreNames.end()))
			throw DatabaseException("NOT_FOUND_ERR", DatabaseException::NOT_FOUND_ERR);

		ObjectStoreSyncList objectStores;
		for_each(storeNames->begin(), storeNames->end(), 
			MapObjectStoreNameToObjectStoreFunctor(*this, objectStores));
		
		currentTransaction = FB::AutoPtr<TransactionSync>(new TransactionSync(*this, transactionFactory, objectStores, timeout));
		}
	else
		currentTransaction = FB::AutoPtr<TransactionSync>(new TransactionSync(*this, transactionFactory, ObjectStoreSyncList(), timeout));

	if(!currentTransaction.is_initialized())
		throw DatabaseException("UNKNOWN_ERR", DatabaseException::UNKNOWN_ERR );
	else
		return currentTransaction.get();
	}

optional<TransactionSync&> DatabaseSync::getCurrentTransaction() const
	{
	return currentTransaction.is_initialized()
		? *currentTransaction.get()
		: optional<TransactionSync&>();
	}

FB::variant DatabaseSync::getCurrentTransactionVariant() const
	{ 
	return currentTransaction.is_initialized()
		? static_cast<FB::JSOutObject>(currentTransaction.get())
		: FB::variant();
	}

void DatabaseSync::onTransactionCommitted(const Transaction& transaction)
	{ 
	openObjectStores.raiseTransactionCommitted(transaction);
	this->currentTransaction.reset(); 
	}

void DatabaseSync::onTransactionAborted(const Transaction& transaction)
	{ 
	openObjectStores.raiseTransactionAborted(transaction);
	this->currentTransaction.reset(); 
	}

const std::string DatabaseSync::getOrigin()
	{ return host->getDOMDocument().getProperty<string>("domain"); }

}
}
}