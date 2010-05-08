/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYDATABASEFACTORY_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYDATABASEFACTORY_H

#include "../AbstractDatabaseFactory.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB {

	class BerkeleyDatabaseFactory : public AbstractDatabaseFactory
		{
		public:
			BerkeleyDatabaseFactory(void) { }
			~BerkeleyDatabaseFactory(void) { }

			virtual std::auto_ptr<Database> createDatabase(const std::string& origin, const std::string& name, const std::string& description, const bool modifyDatabase = true);
			virtual std::auto_ptr<ObjectStore> createObjectStore(Database& database, const std::string& name, const bool autoIncrement = true, TransactionContext& transactionContext = TransactionContext());
			virtual std::auto_ptr<ObjectStore> openObjectStore(Database& database, const std::string& name, const ObjectStore::Mode mode, TransactionContext& transactionContext);		
			
			virtual std::auto_ptr<Index> createIndex(ObjectStore& objectStore, const std::string& name, const std::auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext);
			virtual std::auto_ptr<Index> createIndex(ObjectStore& objectStore, const std::string& name, const bool unique, TransactionContext& transactionContext);
			virtual std::auto_ptr<Index> openIndex(ObjectStore& objectStore, const std::string& name, const std::auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext);
			virtual std::auto_ptr<Index> openIndex(ObjectStore& objectStore, const std::string& name, const bool unique, TransactionContext& transactionContext);

			virtual std::auto_ptr<Cursor> openCursor(ObjectStore& objectStoreSync, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext);
			virtual std::auto_ptr<Cursor> openCursor(Index& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool returnkeys, TransactionContext& transactionContext);
			
			virtual std::auto_ptr<Transaction> createTransaction(Database& database, const ObjectStoreImplementationList& objectStores, const boost::optional<unsigned int>& timeout, TransactionContext& transactionContext);
		};
	}
}
}
}

#endif