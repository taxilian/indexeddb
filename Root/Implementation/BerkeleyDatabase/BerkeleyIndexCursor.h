#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYINDEXCURSOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYINDEXCURSOR_H

#include "BerkeleyCursor.h"
#include "../Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
	
	class Key;

	namespace BerkeleyDB {

		class BerkeleyIndex;

		class BerkeleyIndexCursor : public BerkeleyCursor
			{
			public:
				BerkeleyIndexCursor(BerkeleyIndex& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool dataArePrimaryKeys, TransactionContext& transactionContext);
				virtual ~BerkeleyIndexCursor() { }

				virtual Data getData(TransactionContext& transactionContext);

			private:
				const bool dataArePrimaryKeys;
			};
		}
	}
}
}

#endif