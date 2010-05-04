/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYTRANSACTION_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYTRANSACTION_H

#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include <list>
#include "../Transaction.h"
#include "../ObjectStore.h"

class ::DbTxn;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB {

	class BerkeleyDatabase;

	class BerkeleyTransaction : public Transaction
		{
		public:
			BerkeleyTransaction(BerkeleyDatabase& database, const ObjectStoreImplementationList& objectStores, const boost::optional<unsigned int>& timeout, TransactionContext& transactionContext);
			virtual ~BerkeleyTransaction();

			virtual void commit();
			virtual void abort();

			static DbTxn* ToDbTxn(TransactionContext& transactionContext)
				{ return transactionContext.is_initialized() 
					&& static_cast<BerkeleyTransaction&>(transactionContext.get()).isActive()
						? static_cast<BerkeleyTransaction&>(transactionContext.get()).transaction
						: NULL; }
			
		private:
			DbTxn* transaction;
			boost::mutex synchronization;

			const bool isActive() const { return transaction != NULL; }
		};
	}
}
}
}

#endif