/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYMANUALINDEX_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYMANUALINDEX_H

#include <string>
#include <db_cxx.h>
#include <boost/thread/mutex.hpp>
#include "../Index.h"
#include "../Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB {

	class BerkeleyObjectStore;

	class BerkeleyManualIndex : public Index
		{
		public:
			BerkeleyManualIndex(BerkeleyObjectStore& objectStore, const std::string& name, const bool unique, TransactionContext& transactionContext, const bool create);
			~BerkeleyManualIndex(void);

			virtual Data get(const Key& key, TransactionContext& transactionContext);
			virtual Key getPrimaryKey(const Key& key, TransactionContext& transactionContext);
			virtual void put(const Key& key, const Data& data, const bool noOverwrite, TransactionContext& transactionContext);
			virtual void remove(const Key& key, TransactionContext& transactionContext);
			virtual void close();

		private:
			BerkeleyObjectStore& objectStore;
			Db implementation;
			volatile bool isOpen;
			boost::mutex synchronization;

			Key ensurePrimaryKeyExists(const Dbt& primaryKeyDbt, const Key& secondaryKey, TransactionContext& transactionContext);
			Data ensurePrimaryKeyExists(const Data& data, const Key& secondaryKey, TransactionContext& transactionContext);

			static int callback(Db *secondary, const Dbt *key, const Dbt *data, Dbt *result);

			friend class BerkeleyManualIndexCursor;
		};
	}
}
}
}

#endif