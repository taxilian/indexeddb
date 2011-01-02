/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <BrowserObjectAPI.h>
#include "ObjectStoreSync.h"
#include "DatabaseSync.h"
#include "../DatabaseException.h"
#include "../../Implementation/Database.h"
#include "../../Implementation/AbstractDatabaseFactory.h"
#include "../../Implementation/Data.h"
#include "../../Support/Convert.h"
#include "../../Support/KeyPathKeyGenerator.h"
#include "../../Support/privateObservable.h"

using std::auto_ptr;
using std::string;
using boost::optional;
using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::Data;
using Implementation::ImplementationException;
using Implementation::TransactionContext;
using Implementation::AbstractDatabaseFactory;

namespace API { 

void ObjectStoreSync::addLifeCycleObserver( const LifeCycleObserverPtr& observer )
{
    _observable->addLifeCycleObserver(observer);
}

void ObjectStoreSync::removeLifeCycleObserver( const LifeCycleObserverPtr& observer )
{
    _observable->removeLifeCycleObserver(observer);
}

ObjectStoreSync::ObjectStoreSync(const FB::BrowserHostPtr& host, const DatabaseSyncPtr& database, TransactionFactory& transactionFactory, TransactionContext& transactionContext, Metadata& metadata, const string& name, const string& keyPath, const bool autoIncrement)
	:	ObjectStore(name, Implementation::ObjectStore::READ_WRITE),
		host(host), 
	    transactionFactory(transactionFactory),
		metadata(metadata, Metadata::ObjectStore, name),
		implementation(AbstractDatabaseFactory::getInstance().createObjectStore(
			transactionFactory.getDatabaseContext(), name, autoIncrement, transactionContext))
	{ 
	initializeMethods(); 
	createMetadata(keyPath, autoIncrement, transactionContext);
	}

ObjectStoreSync::ObjectStoreSync(const FB::BrowserHostPtr& host, const DatabaseSyncPtr& database, TransactionFactory& transactionFactory, TransactionContext& transactionContext, Metadata& metadata, const string& name, const bool autoIncrement)
	:	ObjectStore(name, Implementation::ObjectStore::READ_WRITE),
		host(host), 
	    transactionFactory(transactionFactory),
		metadata(metadata, Metadata::ObjectStore, name),
		implementation(AbstractDatabaseFactory::getInstance().createObjectStore(
			transactionFactory.getDatabaseContext(), name, autoIncrement, transactionContext))
	{ 
	//TODO docs say we open all indexes whenever we open the object store (http://www.oracle.com/technology/documentation/berkeley-db/db/programmer_reference/am_second.html)
	initializeMethods(); 
	createMetadata(boost::optional<string>(), autoIncrement, transactionContext);
	}

ObjectStoreSync::ObjectStoreSync(const FB::BrowserHostPtr& host, const DatabaseSyncPtr& database, TransactionFactory& transactionFactory, TransactionContext& transactionContext, Metadata& metadata, const string& name, const Implementation::ObjectStore::Mode mode)
	:	ObjectStore(name, mode),
		transactionFactory(transactionFactory),
		host(host), 
		metadata(metadata, Metadata::ObjectStore, name),
		implementation(AbstractDatabaseFactory::getInstance().openObjectStore(
			transactionFactory.getDatabaseContext(), name, mode, transactionContext))
	{ 
	initializeMethods(); 
	loadMetadata(transactionContext);
	}

ObjectStoreSync::~ObjectStoreSync(void)
    {
    close();
    FB::ptr_cast<Support::_privateObservable<ObjectStoreSync> >(_observable)->invalidate();
    }


void ObjectStoreSync::initializeMethods()
	{
    _observable = boost::make_shared<Support::_privateObservable<ObjectStoreSync> >(this);
	registerMethod("get", make_method(this, &ObjectStoreSync::get));
	registerMethod("put", make_method(this, &ObjectStoreSync::put));
	registerMethod("remove", make_method(this, &ObjectStoreSync::remove));
	registerMethod("openCursor", make_method(this, static_cast<FB::JSAPIPtr (ObjectStoreSync::*)(const FB::CatchAll &)>(&ObjectStoreSync::openCursor))); 

	registerMethod("createIndex", FB::make_method(this, static_cast<FB::JSAPIPtr (ObjectStoreSync::*)(const string, const FB::CatchAll &)>(&ObjectStoreSync::createIndex)));
	registerMethod("openIndex", FB::make_method(this, &ObjectStoreSync::openIndex));
	registerMethod("removeIndex", FB::make_method(this, static_cast<void (ObjectStoreSync::*)(const string&)>(&ObjectStoreSync::removeIndex)));
	}

FB::variant ObjectStoreSync::get(FB::variant key)
	{ 
	try
		{ 
		Data& data = implementation->get(Convert::toKey(host, key), transactionFactory.getTransactionContext()); 

		if(data.getType() == Data::Undefined)
			throw DatabaseException("NOT_FOUND_ERR", DatabaseException::NOT_FOUND_ERR);
		else
			return Convert::toVariant(host, data); 
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant ObjectStoreSync::put(FB::variant value, const FB::CatchAll& args) 
	{ 
	const FB::VariantList& values = args.value;

	if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<bool>())
		throw FB::invalid_arguments();
	else if(this->getMode() != Implementation::ObjectStore::READ_WRITE)
		throw DatabaseException("NOT_ALLOWED_ERR", DatabaseException::NOT_ALLOWED_ERR);
	else if(values.size() >= 1 && !values[0].empty() && keyPath.is_initialized())
		throw DatabaseException("DATA_ERR", DatabaseException::DATA_ERR);

	FB::variant key = values.size() >= 1 && !values[0].empty() ? values[0] : generateKey(value);
	bool noOverwrite = values.size() == 2 ? values[1].cast<bool>() : false;

	if(key.empty())
		throw DatabaseException("DATA_ERR", DatabaseException::DATA_ERR);

	try
		{ implementation->put(Convert::toKey(host, key), Convert::toData(host, value), noOverwrite, transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	
	return key;
	}

void ObjectStoreSync::remove(FB::variant key)
	{ 
	try
		{ implementation->remove(Convert::toKey(host, key), transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void ObjectStoreSync::close()
	{ 
	lock_guard<mutex> guard(synchronization);

	_observable->raiseOnCloseEvent();
	openCursors.release();
	openIndexes.release();
	this->implementation->close(); 
	}


FB::JSAPIPtr ObjectStoreSync::openCursor(const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

    if (!values.size() || values[0].empty())
        return openCursor(KeyRangePtr(), Cursor::NEXT);

	if(values.size() > 2 || !values.size())
		throw FB::invalid_arguments();

    int flags = 0;
    try {
        FB::VariantMap info = values[0].convert_cast<FB::VariantMap>();
        if (info.find("right") == info.end() |
            info.find("left") == info.end() |
            info.find("flags") == info.end()) {
            throw FB::invalid_arguments();
        }
        if (values.size() > 1 && values[1].can_be_type<int>()) {
            flags = values[1].convert_cast<int>();
        } else if(values.size() == 2) {
            flags = info["flags"].convert_cast<int>();
        }

        KeyRangePtr range(boost::make_shared<KeyRange>(info["left"], info["right"], flags));
    	const Cursor::Direction direction = values.size() == 2 ? static_cast<Cursor::Direction>(flags) : Cursor::NEXT;

    	return openCursor(range, direction);
        }
    catch (const FB::bad_variant_cast&)
        {
        throw FB::invalid_arguments();
        }
	}

boost::shared_ptr<CursorSync> ObjectStoreSync::openCursor(const KeyRangePtr& range, const Cursor::Direction direction)
	{
	try
		{ 
		CursorSyncPtr cursor(
            new CursorSync(host, FB::ptr_cast<ObjectStoreSync>(shared_from_this()), transactionFactory, range, direction)
            );
		openCursors.add(cursor);
		return cursor;
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}


FB::JSAPIPtr ObjectStoreSync::createIndex(const string name, const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() >= 1 && !(values[0].is_of_type<string>() || values[0].empty()))
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<bool>())
		throw FB::invalid_arguments();

	optional<string> keyPath = values.size() >= 1 && !values[0].empty() ? values[0].cast<string>() : optional<string>();
	bool unique = values.size() == 2 ? values[1].cast<bool>() : false;

	return static_cast<FB::JSAPIPtr>(createIndex(name, keyPath, unique));
	}

boost::shared_ptr<IndexSync> ObjectStoreSync::createIndex(const string name, const optional<string> keyPath, bool unique)
	{
	metadata.addToMetadataCollection("indexes", name, transactionFactory, transactionFactory.getTransactionContext());

	IndexSyncPtr index(
        new IndexSync(host, FB::ptr_cast<ObjectStoreSync>(shared_from_this()), transactionFactory, metadata, name, keyPath, unique)
        );
	openIndexes.add(index);
	return index;
	}

FB::JSAPIPtr ObjectStoreSync::openIndex(const string& name)
	{
	try
		{
		IndexSyncPtr index(
            new IndexSync(host, FB::ptr_cast<ObjectStoreSync>(shared_from_this()), transactionFactory, metadata, name)
            );
		openIndexes.add(index);
		return static_cast<FB::JSAPIPtr>(index);
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void ObjectStoreSync::removeIndex(const string& indexName)
	{
	try
		{ 
		auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();

		openIndexes.remove(indexName);
		metadata.removeFromMetadataCollection("indexes", indexName, transactionFactory, *transaction);
		getImplementation().removeIndex(indexName, *transaction); 

		transaction->commit();
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant ObjectStoreSync::generateKey(FB::variant value)
	{ 
	FB::variant key;

	if(keyPath.is_initialized())
		key = Support::KeyPathKeyGenerator(host, keyPath.get()).generateKey(value);

	if(key.empty())
		{
		//TODO should have used a BDB sequence here
		//TODO: We have a race condition here... transaction? (probably need to not bother storing key in object, just in db)
		key = (int)(++nextKey);
		metadata.putMetadata("nextKey", Data(&nextKey, sizeof(long), Data::Integer), transactionFactory.getTransactionContext());
		}

	return key;
	}

void ObjectStoreSync::createMetadata(const boost::optional<string>& keyPath, const bool autoIncrement, TransactionContext& transactionContext)
	{
	this->keyPath = keyPath;
	this->autoIncrement = autoIncrement;
	this->nextKey = 0;

	auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction(transactionContext);

	metadata.putMetadata("autoIncrement", Data(&autoIncrement, sizeof(bool), Data::Boolean), true, *transaction);
	metadata.putMetadata("keyPath", keyPath.is_initialized() ? Data(keyPath.get()) : Data::getUndefinedData(), true, *transaction);
	metadata.putMetadata("nextKey", Data(&nextKey, sizeof(long), Data::Integer), true, *transaction);

	transaction->commit();
	}

void ObjectStoreSync::loadMetadata(TransactionContext& transactionContext)
	{
	auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction(transactionContext);

	Data keyPathValue(metadata.getMetadata("keyPath", *transaction));
	keyPath = keyPathValue.getType() == Data::Undefined 
		? optional<string>()
		: optional<string>((char*)keyPathValue.getRawValue());
	autoIncrement = *(bool*)metadata.getMetadata("autoIncrement", *transaction).getRawValue();
	nextKey = *(long*)metadata.getMetadata("nextKey", *transaction).getRawValue();

	transaction->commit();
	}

StringVector ObjectStoreSync::getIndexNames() const
	{ return metadata.getMetadataCollection("indexes", transactionFactory.getTransactionContext()); } 

void ObjectStoreSync::onTransactionCommitted(const TransactionPtr& transaction)
	{ 
	openCursors.raiseTransactionCommitted(transaction); 
	openIndexes.raiseTransactionCommitted(transaction); 
	openCursors.release();
	}

void ObjectStoreSync::onTransactionAborted(const TransactionPtr& transaction)
	{ 
	openCursors.raiseTransactionAborted(transaction); 
	openIndexes.raiseTransactionAborted(transaction); 
	openCursors.release();
	}

}
}
}