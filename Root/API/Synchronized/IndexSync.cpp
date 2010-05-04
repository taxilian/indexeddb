/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#include "BrowserObjectAPI.h"
#include "ObjectStoreSync.h"
#include "IndexSync.h"
#include "../../Implementation/Index.h"
#include "../../Implementation/KeyGenerator.h"
#include "../../Implementation/AbstractDatabaseFactory.h"
#include "../../Support/KeyGeneratorHelper.h"
#include "../../Support/Convert.h"
#include "../DatabaseException.h"

using std::auto_ptr;
using std::string;
using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::ImplementationException;
using Implementation::KeyGenerator;
using Implementation::Key;
using Implementation::Data;

namespace API { 

IndexSync::IndexSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const string& name)
	: Index(name, objectStore.getName()), 
	  transactionFactory(transactionFactory),
	  metadata(metadata, Metadata::Index, name),
	  host(host)
	{ 
	loadMetadata();

	keyGenerator = auto_ptr<KeyGenerator>(keyPath.is_initialized()
		? new Support::KeyGeneratorHelper(host, keyPath.get())
		: NULL);
	implementation = keyPath.is_initialized()
		? Implementation::AbstractDatabaseFactory::getInstance().openIndex(
			objectStore.getImplementation(), name, keyGenerator, unique, transactionFactory.getTransactionContext())
		: Implementation::AbstractDatabaseFactory::getInstance().openIndex(
			objectStore.getImplementation(), name, unique, transactionFactory.getTransactionContext());
	initializeMethods();
	}

IndexSync::IndexSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const string& name, const optional<string>& keyPath, const bool unique)
	: Index(name, objectStore.getName(), keyPath, unique), 
	  transactionFactory(transactionFactory),
	  host(host),
	  metadata(metadata, Metadata::Index, name),
	  keyGenerator(keyPath.is_initialized()
	  ? new Support::KeyGeneratorHelper(host, keyPath.get())
		: NULL),
	  implementation(keyPath.is_initialized()
		? Implementation::AbstractDatabaseFactory::getInstance().createIndex(
			objectStore.getImplementation(), name, keyGenerator, unique, transactionFactory.getTransactionContext())
		: Implementation::AbstractDatabaseFactory::getInstance().createIndex(
			objectStore.getImplementation(), name, unique, transactionFactory.getTransactionContext()))
	{ 
	createMetadata(keyPath, unique);
	initializeMethods(); 
	}

IndexSync::~IndexSync(void)
	{ close(); }

void IndexSync::initializeMethods()
	{
	registerMethod("get", make_method(this, &IndexSync::get));
	registerMethod("getObject", make_method(this, &IndexSync::getObject));
	registerMethod("put", make_method(this, &IndexSync::put));
	registerMethod("remove", make_method(this, &IndexSync::remove));
	registerMethod("openCursor", make_method(this, static_cast<FB::JSOutObject (IndexSync::*)(const FB::CatchAll &)>(&IndexSync::openCursor))); 
	registerMethod("openObjectCursor", make_method(this, static_cast<FB::JSOutObject (IndexSync::*)(const FB::CatchAll &)>(&IndexSync::openObjectCursor))); 
	}

FB::variant IndexSync::get(FB::variant key)
	{ 
	try
		{ 
		Key& primaryKey = implementation->getPrimaryKey(Convert::toKey(host, key), transactionFactory.getTransactionContext()); 

		if(primaryKey.getType() == Data::Undefined)
			throw DatabaseException("NOT_FOUND_ERR", DatabaseException::NOT_FOUND_ERR);
		else
			return Convert::toVariant(host, primaryKey); 
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant IndexSync::getObject(FB::variant key)
	{
	try
		{
		Data& data = implementation->get(Convert::toKey(host, key), transactionFactory.getTransactionContext());

		if(data.getType() == Data::Undefined)
			throw DatabaseException(DatabaseException::NOT_FOUND_ERR);
		else
			return Convert::toVariant(host, data); 
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant IndexSync::put(FB::variant value, const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(keyPath.is_initialized())
		throw DatabaseException("CONSTRAINT_ERR", DatabaseException::CONSTRAINT_ERR);
	else if(values.size() < 1)
		throw FB::invalid_arguments();
	else if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<bool>())
		throw FB::invalid_arguments();
 
	FB::variant key = values[0];
	bool noOverwrite = values.size() == 2 ? values[1].cast<bool>() : false;

	try
		{ implementation->put(Convert::toKey(host, key), Convert::toData(host, value), noOverwrite, transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }

	return key;
	}

FB::variant IndexSync::remove(FB::variant key)
	{
	//TODO return void
	try
		{ implementation->remove(Convert::toKey(host, key), transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	return 0;
	}

void IndexSync::close()
	{ 
	this->raiseOnCloseEvent();
	openCursors.release();
	this->implementation->close(); 
	}

FB::JSOutObject IndexSync::openCursor(const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() >= 1 && !values[0].empty() &&
			(!values[0].is_of_type<FB::JSObject>() ||
 			 !values[0].cast<FB::JSObject>()->HasProperty("left") || 
			 !values[0].cast<FB::JSObject>()->HasProperty("right") ||
			 !values[0].cast<FB::JSObject>()->HasProperty("flags") ||
			 !values[0].cast<FB::JSObject>()->GetProperty("flags").is_of_type<int>()))
				throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<int>())
		throw FB::invalid_arguments();

	optional<KeyRange> range = values.size() >= 1 && !values[0].empty()
		? KeyRange(values[0].cast<FB::JSObject>()->GetProperty("left"),
			values[0].cast<FB::JSObject>()->GetProperty("right"),
			values[0].cast<FB::JSObject>()->GetProperty("flags").cast<int>())
		: optional<KeyRange>();
	const Cursor::Direction direction = values.size() == 2 ? static_cast<Cursor::Direction>(values[1].cast<int>()) : Cursor::NEXT;

	return static_cast<FB::JSOutObject>(openCursor(range, direction, true));
	}

FB::JSOutObject IndexSync::openObjectCursor(const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() >= 1 && 
		(!values[0].is_of_type<FB::JSObject>() ||
 		 !values[0].cast<FB::JSObject>()->HasProperty("left") || 
		 !values[0].cast<FB::JSObject>()->HasProperty("right") ||
		 !values[0].cast<FB::JSObject>()->HasProperty("flags") ||
		 !values[0].cast<FB::JSObject>()->GetProperty("flags").is_of_type<int>()))
			throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<int>())
		throw FB::invalid_arguments();

	optional<KeyRange> range = values.size() >= 1
		? KeyRange(values[0].cast<FB::JSObject>()->GetProperty("left"),
			values[0].cast<FB::JSObject>()->GetProperty("right"),
			values[0].cast<FB::JSObject>()->GetProperty("flags").cast<int>())
		: optional<KeyRange>();
	const Cursor::Direction direction = values.size() == 2 ? static_cast<Cursor::Direction>(values[1].cast<int>()) : Cursor::NEXT;

	return static_cast<FB::JSOutObject>(openCursor(range, direction, false));
	}

FB::AutoPtr<CursorSync> IndexSync::openCursor(const optional<KeyRange>& range, const Cursor::Direction direction, const bool dataArePrimaryKeys)
	{ 
	try
		{ 
		FB::AutoPtr<CursorSync> cursor = new CursorSync(host, *this, transactionFactory, range, direction, dataArePrimaryKeys); 
		openCursors.add(cursor);
		return cursor;
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void IndexSync::createMetadata(const optional<string>& keyPath, const bool unique)
	{
	this->keyPath = keyPath;
	this->unique = unique;

	auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();

	metadata.putMetadata("unique", Data(&unique, sizeof(bool), Data::Boolean), true, *transaction);
	metadata.putMetadata("keyPath", keyPath.is_initialized() ? Data(keyPath.get()) : Data::getUndefinedData(), true, *transaction);
	
	transaction->commit();
	}

void IndexSync::loadMetadata()
	{
	auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();

	Data keyPathValue(metadata.getMetadata("keyPath", *transaction));
	this->keyPath = keyPathValue.getType() == Data::Undefined 
		? optional<string>()
		: optional<string>((char*)keyPathValue.getUntypedValue());
	this->unique = *(bool*)metadata.getMetadata("unique", *transaction).getUntypedValue();

	transaction->commit();
	}

void IndexSync::onTransactionCommitted(const Transaction& transaction)
	{ 
	openCursors.raiseTransactionCommitted(transaction); 
	openCursors.release();
	}

void IndexSync::onTransactionAborted(const Transaction& transaction)
	{ 
	openCursors.raiseTransactionAborted(transaction); 
	openCursors.release();
	}

}
}
}