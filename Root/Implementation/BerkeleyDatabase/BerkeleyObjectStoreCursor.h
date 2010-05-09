/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

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

		///<summary>
		/// This class represents a cursor over an object store backed by a Berkeley DB database
		///</summary>
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