/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_DATABASE_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_DATABASE_H

#include "Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class ObjectStore;

	///<summary>
	/// This class represents a data value in the Indexed Database API implementation.  Data instances
	/// are exchanged between the API layer and implementation layer.
	///</summary>
	class Database
		{
		public:
			virtual ~Database() { }

			/// Removes the given object store from the database
			virtual void removeObjectStore(const std::string& objectStoreName, TransactionContext& transactionContext) = 0;
			
			// Gets the metadata associated with this database
			virtual ObjectStore& getMetadata() = 0;
		};
	}
}
}

#endif