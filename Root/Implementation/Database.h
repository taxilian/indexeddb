#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_DATABASE_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_DATABASE_H

#include "Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class ObjectStore;

	class Database
		{
		public:
			virtual ~Database() { }

			virtual void removeObjectStore(const std::string& objectStoreName, TransactionContext& transactionContext) = 0;
			virtual ObjectStore& getMetadata() = 0;
		};
	}
}
}

#endif