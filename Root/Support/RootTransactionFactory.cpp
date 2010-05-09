/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "RootTransactionFactory.h"
#include "../API/Synchronized/TransactionSync.h"
#include "../API/Synchronized/DatabaseSync.h"
#include "../Implementation/Database.h"

using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::TransactionContext;

namespace API { 

Implementation::TransactionContext RootTransactionFactory::getTransactionContext() const
	{ 
	return database.currentTransaction.is_initialized()
		? (database.currentTransaction.get())->getTransactionContext()
		: TransactionContext();
	}

}
}
}