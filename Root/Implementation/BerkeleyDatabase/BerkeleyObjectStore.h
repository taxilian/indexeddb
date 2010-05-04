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

				Db& getImplementation() { return implementation; }

			private:
				Db implementation;
				const bool readOnly;
				volatile bool isOpen;

				boost::mutex synchronization;
			};
		}
	}
}
}

#endif