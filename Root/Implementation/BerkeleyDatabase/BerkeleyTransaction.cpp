/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <db_cxx.h>
#include "BerkeleyTransaction.h"
#include "BerkeleyDatabase.h"
#include "../ObjectStore.h"
#include "../ImplementationException.h"

using ::std::list;
using ::boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyTransaction::BerkeleyTransaction(BerkeleyDatabase& database, const ObjectStoreImplementationList& objectStores, const optional<unsigned int>& timeout, TransactionContext& transactionContext)
		{
		//TODO We're not locking the object stores per spec
		try
			{ 
			database.getEnvironment().txn_begin(ToDbTxn(transactionContext), &transaction, 0); 
			if(timeout.is_initialized()) transaction->set_timeout(timeout.get(), DB_SET_TXN_TIMEOUT);
			}
		catch(DbException e)
			{ throw ImplementationException(e.what(), ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	BerkeleyTransaction::~BerkeleyTransaction()
		{ 
		try
			{ abort(); }
		// Shouldn't be throwing in destructors, and there's really nothing that can be done here anyway
		catch(ImplementationException&)
			{ }
		}

	void BerkeleyTransaction::commit()
		{ 
		if(!isActive())
			throw ImplementationException(ImplementationException::NON_TRANSIENT_ERR);

		try
			{ transaction->commit(0); }
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }

		transaction = NULL;
		}

	void BerkeleyTransaction::abort()
		{ 
		if(!isActive())
			throw ImplementationException(ImplementationException::NON_TRANSIENT_ERR);
		
		DbTxn* transaction(this->transaction);
		this->transaction = NULL;

		try
			{ transaction->abort(); }
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}
	}
}
}
}