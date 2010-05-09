/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
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

	///<summary>
	/// This class represents a manual index to an object store (the spec calls them non-auto-populated indices).
	/// We use an unassociated Berkeley DB for the index; keys are synchronized automatically, during get and cursor
	/// operations.  This is a little bit inefficient, but the spec requires we NOT automatically delete primary
	/// keys during an index delete, and Berkeley DB does not support this.  So we do things the hard way.
	///</summary>
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
			// The object store with which this association is associated
			BerkeleyObjectStore& objectStore;
			// The underlying object store that represents this index
			Db implementation;
			// Flag indicating whether the index is open
			volatile bool isOpen;

			// Some of our operations must be synchronized for thread safety
			boost::mutex synchronization;

			// Helper methods to ensure primary keys exist; we need this to maintain consistency between
			// the index and the associated object store.
			Key ensurePrimaryKeyExists(const Dbt& primaryKeyDbt, const Key& secondaryKey, TransactionContext& transactionContext);
			Data ensurePrimaryKeyExists(const Data& data, const Key& secondaryKey, TransactionContext& transactionContext);

			// A cursor over this index will need access to our implementation
			friend class BerkeleyManualIndexCursor;
		};
	}
}
}
}

#endif