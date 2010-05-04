#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYOBJECTSTORECURSOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYOBJECTSTORECURSOR_H

#include "BerkeleyCursor.h"
#include "../Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
	
	class Key;
	
	namespace BerkeleyDB {

		class BerkeleyObjectStore;

		class BerkeleyObjectStoreCursor : public BerkeleyCursor
			{
			public:
				BerkeleyObjectStoreCursor(BerkeleyObjectStore& objectStore, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext);
			};
		}
	}
}
}

#endif