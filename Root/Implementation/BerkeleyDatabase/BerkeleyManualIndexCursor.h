/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

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

		///<summary>
		/// This class represents a cursor over a manually synchronized index.  Before returning a cursor value,
		/// we are required to manually ensure that the primary key actually exists.
		///</summary>
		class BerkeleyManualIndexCursor : public BerkeleyCursor
			{
			public:
				BerkeleyManualIndexCursor(BerkeleyManualIndex& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool dataArePrimaryKeys, TransactionContext& transactionContext);
				~BerkeleyManualIndexCursor() { }

				virtual Data getData(TransactionContext& transactionContext);
				virtual void remove();

			protected:
				// Override of the base implementation; we ensure the key exists before deferring
				virtual bool next(Dbc* cursor, TransactionContext& transactionContext);

			private:
				// The index with which this cursor is associated
				BerkeleyManualIndex& index;
				// A cache of the current value of the cursor
				Data currentValue;
				// Flag indicating whether primary keys or primary values are returned on a get
				const bool dataArePrimaryKeys;

				// Helper method used to ensure a primary key actually exists while iterating the cursor
				bool ensurePrimaryKeyExists(TransactionContext& transactionContext);
			};
		}
	}
}
}

#endif