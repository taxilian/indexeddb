/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYOBJECTSTORE_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYOBJECTSTORE_H

#include <string>
#include <boost/thread/mutex.hpp>
#include <db_cxx.h>
#include "../ObjectStore.h"

namespace boost { class mutex; }

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
	
	class Key;
	class Data;
	
	namespace BerkeleyDB {
		class BerkeleyDatabase;

		///<summary>
		/// This class represents an Indexed Database API object store; it is backed by a Berkeley DB database.
		///</summary>
		class BerkeleyObjectStore : public ObjectStore
			{
			public:
				BerkeleyObjectStore(BerkeleyDatabase& database, const std::string& name, const bool autoIncrement, TransactionContext& transactionContext);
				BerkeleyObjectStore(BerkeleyDatabase& database, const std::string& name, const Mode mode, const bool create, TransactionContext& transactionContext);
				~BerkeleyObjectStore(void);

				virtual Data get(const Key& key, TransactionContext& transactionContext);
				virtual void put(const Key& key, const Data& data, const bool noOverwrite, TransactionContext& transactionContext);
				virtual bool exists(const Key& key, TransactionContext& transactionContext);
				virtual void remove(const Key& key, TransactionContext& transactionContext);
				virtual void close();
		
				virtual void removeIndex(const std::string& name, TransactionContext& transactionContext);

				/// Get the underlying implementation associated with this object store.  Would have
				/// preferred to have not exposed this, but that would have required lots of friends.
				Db& getImplementation() { return implementation; }

			private:
				// Our backing Berkeley DB database for this object store
				Db implementation;
				// Flag indicating whether this object store is read-only
				const bool readOnly;
				// Flag indicating whether this object store is still open
				volatile bool isOpen;

				// Used to thread-synch critical sections
				boost::mutex synchronization;
			};
		}
	}
}
}

#endif