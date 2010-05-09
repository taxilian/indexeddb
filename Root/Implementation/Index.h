/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

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

	///<summary>
	/// This class represents the implementation of an index in the Indexed Database API.  
	/// Most of its functionality is identical to the object store, so it also implements this interface.
	///</summary>
	class Index : public ObjectStore
		{
		public:
			virtual ~Index() { }

			// Gets the primary key associated with a secondary key (as opposed to the primary value)
			virtual Key getPrimaryKey(const Key& secondaryKey, TransactionContext& transactionContext) = 0;

		private:
			// We don't implement this part of the object store interface, so shield it from consumers
			void removeIndex(const std::string& name, TransactionContext& transactionContext) 
				{ throw ImplementationException(ImplementationException::NOT_ALLOWED_ERR); } 
		};
	}
}
}

#endif