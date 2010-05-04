/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_OBJECTSTORE_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_OBJECTSTORE_H

#include <list>
#include <boost/optional.hpp>
#include "Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Key;
	class Data;

	class ObjectStore
		{
		public:
			enum Mode { READ_WRITE = 0, READ_ONLY = 1, SNAPSHOT_READ = 2 };

			virtual ~ObjectStore() { }

			virtual Data get(const Key& key, TransactionContext& transactionContext) = 0;
			virtual void put(const Key& key, const Data& data, const bool noOverwrite, TransactionContext& transactionContext) = 0;
			virtual void remove(const Key& key, TransactionContext& transactionContext) = 0;
			virtual void close() = 0;

			virtual void removeIndex(const std::string& name, TransactionContext& transactionContext) = 0;
		};

	typedef boost::optional<std::list<ObjectStore*>> ObjectStoreImplementationList;
	}
}
}

#endif