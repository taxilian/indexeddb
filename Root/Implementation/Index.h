#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_INDEX_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_INDEX_H

#include "ObjectStore.h"
#include "Transaction.h"
#include "ImplementationException.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Data;
	class Key;

	class Index : public ObjectStore
		{
		public:
			virtual ~Index() { }

			virtual Key getPrimaryKey(const Key& secondaryKey, TransactionContext& transactionContext) = 0;

		private:
			void removeIndex(const std::string& name, TransactionContext& transactionContext) 
				{ throw ImplementationException(ImplementationException::NOT_ALLOWED_ERR); } 
		};
	}
}
}

#endif