#include "RootTransactionFactory.h"
#include "../Bridge/Sync/TransactionSync.h"
#include "../Bridge/Sync/DatabaseSync.h"
#include "../Implementation/Database.h"

using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::TransactionContext;

namespace API { 

Implementation::TransactionContext RootTransactionFactory::getTransactionContext() const
	{ 
	return database.getCurrentTransaction().is_initialized()
		? database.getCurrentTransaction()->getTransactionContext()
		: TransactionContext();
	}

}
}
}