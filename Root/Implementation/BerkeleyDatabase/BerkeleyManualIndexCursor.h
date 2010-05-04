#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYMANUALINDEXCURSOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYMANUALINDEXCURSOR_H

#include "BerkeleyCursor.h"
#include "../Data.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
	
	class Key;
	
	namespace BerkeleyDB {

		class BerkeleyManualIndex;

		class BerkeleyManualIndexCursor : public BerkeleyCursor
			{
			public:
				BerkeleyManualIndexCursor(BerkeleyManualIndex& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool dataArePrimaryKeys, TransactionContext& transactionContext);
				~BerkeleyManualIndexCursor() { }

				virtual Data getData(TransactionContext& transactionContext);
				virtual void remove();

			protected:
				virtual bool next(Dbc* cursor, TransactionContext& transactionContext);

			private:
				BerkeleyManualIndex& index;
				Data currentValue;
				const bool dataArePrimaryKeys;

				bool ensurePrimaryKeyExists(TransactionContext& transactionContext);
			};
		}
	}
}
}

#endif