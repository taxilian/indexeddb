/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <DOM/Document.h>
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

BrandonHaynes::IndexedDB::API::DatabaseSyncPtr DatabaseSync::create( FB::BrowserHostPtr host, const std::string& name, const std::string& description, const bool modifyDatabase )
    {
    DatabaseSyncPtr ptr(new DatabaseSync(host, name, description, modifyDatabase));
    ptr->init();
    return ptr;
    }

void DatabaseSync::init()
    {
    transactionFactory.setDatabaseSync(FB::ptr_cast<DatabaseSync>(shared_from_this()));
    }

DatabaseSync::DatabaseSync(FB::BrowserHostPtr host, const string& name, const string& description, const bool modifyDatabase)
	: Database(name, description),
	  host(host), 
	  modifyDatabase(modifyDatabase),
      openObjectStores(boost::make_shared<Support::Container<ObjectStoreSync> >()),
	  implementation(Implementation::AbstractDatabaseFactory::getInstance()
		.createDatabase(getOrigin(), name, description, modifyDatabase)),
	  metadata(implementation->getMetadata(), Metadata::Database, name),
	  #pragma warning(push)
	  #pragma warning(disable: 4355)
		transactionFactory(*implementation)
	  #pragma warning(pop)
	{
	registerMethod("createObjectStore", make_method(this, &DatabaseSync::createObjectStore)); 
	registerMethod("openObjectStore", make_method(this, static_cast<FB::JSAPIPtr (DatabaseSync::*)(const string&, const FB::CatchAll &)>(&DatabaseSync::openObjectStore))); 
	registerMethod("removeObjectStore", FB::make_method(this, &DatabaseSync::removeObjectStore));
	registerMethod("setVersion", FB::make_method(this, &DatabaseSync::setVersion));
	registerMethod("transaction", FB::make_method(this, static_cast<TransactionSyncPtr (DatabaseSync::*)(const FB::variant&, const boost::optional<unsigned int>)>(&DatabaseSync::transaction))); 
	}

DatabaseSync::~DatabaseSync()
	{
	if(currentTransaction)
		currentTransaction->close();

	openObjectStores->release();
	}

FB::JSAPIPtr DatabaseSync::openObjectStore(const string& name, const FB::CatchAll& args)
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

	return static_cast<FB::JSAPIPtr>(openObjectStore(name, mode));
	}

optional<string> DatabaseSync::getVersion() const
	{ 
	Data version = metadata.getMetadata("version", transactionFactory.getTransactionContext());
	return version.getType() == Data::Undefined
		? optional<string>()
		: Convert::toVariant(host, metadata.getMetadata("version", transactionFactory.getTransactionContext())).cast<string>(); 
	}

void DatabaseSync::setVersion(const string& version)
	{ metadata.putMetadata("version", Data(version), transactionFactory.getTransactionContext()); }

ObjectStoreSyncPtr DatabaseSync::createObjectStore(const string& name, const boost::optional<string>& keyPath, boost::optional<bool> autoIncrement)
	{ 
	ensureCanCreateObjectStore(name);
    bool ai = autoIncrement ? *autoIncrement : false;
    try
        {
        if (keyPath) 
            {
        	metadata.addToMetadataCollection("objectStores", name, transactionFactory, transactionFactory.getTransactionContext());
        	ObjectStoreSyncPtr objectStore(
                new ObjectStoreSync(
                    host,
                    FB::ptr_cast<DatabaseSync>(shared_from_this()),
                    transactionFactory,
                    transactionFactory.getTransactionContext(),
                    metadata,
                    name,
                    *keyPath,
                    ai));
        	openObjectStores->add(objectStore);
            return objectStore;
            }
        else
            {
    		auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();
    		
    		metadata.addToMetadataCollection("objectStores", name, transactionFactory, *transaction);
    		boost::shared_ptr<ObjectStoreSync> objectStore(
                new ObjectStoreSync(
                    host,
                    FB::ptr_cast<DatabaseSync>(shared_from_this()),
                    transactionFactory,
                    *transaction,
                    metadata,
                    name,
                    ai
                ));
    		openObjectStores->add(objectStore);

    		transaction->commit();

    		return objectStore;
            }
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

boost::shared_ptr<ObjectStoreSync> DatabaseSync::openObjectStore(const string& name, Implementation::ObjectStore::Mode mode)
	{  
	if(mode != Implementation::ObjectStore::READ_ONLY 
	   && mode != Implementation::ObjectStore::READ_WRITE 
	   && mode != Implementation::ObjectStore::SNAPSHOT_READ)
		throw FB::invalid_arguments();

	try
		{
		boost::shared_ptr<ObjectStoreSync> objectStore(
            new ObjectStoreSync(
                host,
                FB::ptr_cast<DatabaseSync>(shared_from_this()),
                transactionFactory,
                transactionFactory.getTransactionContext(),
                metadata,
                name,
                mode));
		openObjectStores->add(objectStore);
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

	openObjectStores->remove(storeName);
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

TransactionSyncPtr DatabaseSync::transaction(const string& objectStoreName, const optional<unsigned int>& timeout)
	{ return transaction(StringVector(1, objectStoreName), timeout); }

TransactionSyncPtr DatabaseSync::transaction(const FB::variant& objStoreName, const boost::optional<unsigned int> timeout)
	{
	// We allow a single string, an array, or nothing as possible object store names
	// (Believe spec disallows a single string, but that's silly)
	if(objStoreName.empty())
		transaction(StringVector(), timeout);
	else if(objStoreName.can_be_type<FB::JSObjectPtr>())
		try
			{ transaction(objStoreName.convert_cast<StringVector>(), timeout); }
		catch(FB::bad_variant_cast)
			{ throw FB::invalid_arguments(); }
	else if(objStoreName.can_be_type<string>())
		transaction(objStoreName.convert_cast<string>(), timeout);
	else
		throw FB::invalid_arguments();
	
	if(!currentTransaction)
		throw DatabaseException("UNKNOWN_ERR", DatabaseException::UNKNOWN_ERR );
	else
		return currentTransaction;
	}

boost::shared_ptr<TransactionSync> DatabaseSync::transaction(const StringVector& inStoreNames, const optional<unsigned int>& timeout)
	{
	if(getCurrentTransaction())
		throw DatabaseException("NOT_ALLOWED_ERR", DatabaseException::NOT_ALLOWED_ERR);
	else if(inStoreNames.size())
		{
		StringVector objectStoreNames = getObjectStoreNames();
        StringVector storeNames(inStoreNames);
		sort(storeNames.begin(), storeNames.end());
		sort(objectStoreNames.begin(), objectStoreNames.end());

		// Ensure that the passed-in object store names actually exist in our database
		if(!std::includes(storeNames.begin(), storeNames.end(), objectStoreNames.begin(), objectStoreNames.end()))
			throw DatabaseException("NOT_FOUND_ERR", DatabaseException::NOT_FOUND_ERR);

		ObjectStoreSyncList objectStores;
		for_each(storeNames.begin(), storeNames.end(), 
			MapObjectStoreNameToObjectStoreFunctor(FB::ptr_cast<DatabaseSync>(shared_from_this()), objectStores));
		
		currentTransaction = boost::shared_ptr<TransactionSync>(
            new TransactionSync(FB::ptr_cast<DatabaseSync>(shared_from_this()), transactionFactory, objectStores, timeout)
            );
		}
	else
		currentTransaction = boost::shared_ptr<TransactionSync>(
            new TransactionSync(FB::ptr_cast<DatabaseSync>(shared_from_this()), transactionFactory, ObjectStoreSyncList(), timeout)
            );

	if(!getCurrentTransaction())
		throw DatabaseException("UNKNOWN_ERR", DatabaseException::UNKNOWN_ERR );
	else
		return FB::ptr_cast<TransactionSync>(getCurrentTransaction());
	}

TransactionPtr DatabaseSync::getCurrentTransaction() const
	{
	return currentTransaction;
	}

void DatabaseSync::onTransactionCommitted(const TransactionPtr& transaction)
	{ 
	openObjectStores->raiseTransactionCommitted(transaction);
	this->currentTransaction.reset(); 
	}

void DatabaseSync::onTransactionAborted(const TransactionPtr& transaction)
	{ 
	openObjectStores->raiseTransactionAborted(transaction);
	this->currentTransaction.reset(); 
	}

std::string DatabaseSync::getOrigin()
	{ return host->getDOMDocument()->getProperty<string>("domain"); }

}
}
}