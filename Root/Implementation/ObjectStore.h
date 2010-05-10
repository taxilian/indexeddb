/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
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

	///<summary>
	/// This interface represents the implementation of an Indexed Database API object store
	///</summary>
	class ObjectStore
		{
		public:
			// An enumeration representing the available object store read/write modes
			enum Mode { READ_WRITE = 0, READ_ONLY = 1, SNAPSHOT_READ = 2 };

			virtual ~ObjectStore() { }

			// Get a value from the object store identified by the given key
			virtual Data get(const Key& key, TransactionContext& transactionContext) = 0;
			// Put a value into the object store identified by the given key (possibly overwriting an existing value)
			virtual void put(const Key& key, const Data& data, const bool noOverwrite, TransactionContext& transactionContext) = 0;
			// Remove an item from the object store as identified by a key
			virtual void remove(const Key& key, TransactionContext& transactionContext) = 0;
			// Close this object store
			virtual void close() = 0;

			// Remove an index associated with the object store
			virtual void removeIndex(const std::string& name, TransactionContext& transactionContext) = 0;
		};

	// This typedef represents an optional list of object store implementations.  It is used
	// primarily to manage static transactions.
	typedef boost::optional<std::list<ObjectStore*>> ObjectStoreImplementationList;
	}
}
}

#endif