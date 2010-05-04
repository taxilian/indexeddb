#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYINDEX_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYINDEX_H

#include <memory>
#include <boost/thread/mutex.hpp>
#include <db_cxx.h>
#include "../Index.h"
#include "../Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class KeyGenerator; 
	class Key;
	class Data;

	namespace BerkeleyDB {

		class BerkeleyObjectStore;

		class BerkeleyIndex : public Index
			{
			public:
				BerkeleyIndex(BerkeleyObjectStore& objectStore, const std::string& name, const std::auto_ptr<KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext, const bool create);
				virtual ~BerkeleyIndex(void);

				virtual Data get(const Key& secondaryKey, TransactionContext& transactionContext);
				virtual Key getPrimaryKey(const Key& secondaryKey, TransactionContext& transactionContext);
				virtual void put(const Key& secondaryKey, const Data& primaryKey, const bool noOverwrite, TransactionContext& transactionContext);
				virtual void remove(const Key& secondaryKey, TransactionContext& transactionContext);
				virtual void close();

			private:
				Db implementation;
				volatile bool isOpen;
				boost::mutex synchronization;

				static int callback(Db *secondary, const Dbt *key, const Dbt *data, Dbt *result);

				friend class BerkeleyIndexCursor;
			};
		}
	}
}
}

#endif