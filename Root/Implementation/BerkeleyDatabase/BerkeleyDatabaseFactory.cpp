#include "BerkeleyDatabaseFactory.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyObjectStore.h"
#include "BerkeleyObjectStoreCursor.h"
#include "BerkeleyIndex.h"
#include "BerkeleyIndexCursor.h"
#include "BerkeleyManualIndexCursor.h"
#include "BerkeleyManualIndex.h"
#include "BerkeleyTransaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	using ::std::auto_ptr;
	using ::std::list;
	using ::std::string;
	using ::boost::optional;

	auto_ptr<Database> BerkeleyDatabaseFactory::createDatabase(const string& origin, const string& name, const string& description, const bool modifyDatabase)
		{ return auto_ptr<Database>(new BerkeleyDatabase(origin, name, description, modifyDatabase)); }

	auto_ptr<ObjectStore> BerkeleyDatabaseFactory::createObjectStore(Database& database, const string& name, const bool autoIncrement, TransactionContext& transactionContext)
		{ return auto_ptr<ObjectStore>(new BerkeleyObjectStore(static_cast<BerkeleyDatabase&>(database), name, autoIncrement, transactionContext)); }

	auto_ptr<ObjectStore> BerkeleyDatabaseFactory::openObjectStore(Database& database, const string& name, const ObjectStore::Mode mode, TransactionContext& transactionContext)
		{ return auto_ptr<ObjectStore>(new BerkeleyObjectStore(static_cast<BerkeleyDatabase&>(database), name, mode, false, transactionContext)); }

	auto_ptr<Transaction> BerkeleyDatabaseFactory::createTransaction(Database& database, const ObjectStoreImplementationList& objectStores, const optional<unsigned int>& timeout, TransactionContext& transactionContext)
		{ return auto_ptr<Transaction>(new BerkeleyTransaction(static_cast<BerkeleyDatabase&>(database), objectStores, timeout, transactionContext)); }

	auto_ptr<Index> BerkeleyDatabaseFactory::createIndex(ObjectStore& objectStore, const string& name, const auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext)
		{ return auto_ptr<Index>(new BerkeleyIndex(static_cast<BerkeleyObjectStore&>(objectStore), name, keyGenerator, unique, transactionContext, true)); } 
	
	auto_ptr<Index> BerkeleyDatabaseFactory::createIndex(ObjectStore& objectStore, const string& name, const bool unique, TransactionContext& transactionContext)
		{ return auto_ptr<Index>(new BerkeleyManualIndex(static_cast<BerkeleyObjectStore&>(objectStore), name, unique, transactionContext, true)); } 

	auto_ptr<Index> BerkeleyDatabaseFactory::openIndex(ObjectStore& objectStore, const string& name, const auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext)
		{ return auto_ptr<Index>(new BerkeleyIndex(static_cast<BerkeleyObjectStore&>(objectStore), name, keyGenerator, unique, transactionContext, false)); }

	auto_ptr<Index> BerkeleyDatabaseFactory::openIndex(ObjectStore& objectStore, const string& name, const bool unique, TransactionContext& transactionContext)
		{ return auto_ptr<Index>(new BerkeleyManualIndex(static_cast<BerkeleyObjectStore&>(objectStore), name, unique, transactionContext, false));  }

	auto_ptr<Cursor> BerkeleyDatabaseFactory::openCursor(ObjectStore& objectStore, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext)
		{ return auto_ptr<Cursor>(new BerkeleyObjectStoreCursor(static_cast<BerkeleyObjectStore&>(objectStore), left, right, openLeft, openRight, isReversed, omitDuplicates, transactionContext)); }

	auto_ptr<Cursor> BerkeleyDatabaseFactory::openCursor(Index& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool returnKeys, TransactionContext& transactionContext)
		{ 
		// Not a fan of RTTI here, should probably clean up at some point.
		if(typeid(index) == typeid(BerkeleyIndex&))
			return auto_ptr<Cursor>(new BerkeleyIndexCursor(static_cast<BerkeleyIndex&>(index), left, right, openLeft, openRight, isReversed, omitDuplicates, returnKeys, transactionContext)); 
		else
			return auto_ptr<Cursor>(new BerkeleyManualIndexCursor(static_cast<BerkeleyManualIndex&>(index), left, right, openLeft, openRight, isReversed, omitDuplicates, returnKeys, transactionContext)); 
		}
	}
}
}
}