/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
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

	///<summary>
	/// This class represents a transaction backed by a Berkeley DB transaction; it is used both for
	/// the singleton transaction in the Indexed Database API, and also for other internal operations
	/// that are transactional.  Thus, implementations cannot assumed that it will represent "the" Indexed Database
	/// API singleton.
	///</summary>
	class BerkeleyTransaction : public Transaction
		{
		public:
			BerkeleyTransaction(BerkeleyDatabase& database, const ObjectStoreImplementationList& objectStores, const boost::optional<unsigned int>& timeout, TransactionContext& transactionContext);
			virtual ~BerkeleyTransaction();

			virtual void commit();
			virtual void abort();

			/// Utility method to convert a transaction context into a Berkeley DB DbTxn pointer
			/// This is used through the Berkeley DB implementation; many operations require a DbTxn context
			static DbTxn* ToDbTxn(TransactionContext& transactionContext)
				{ return transactionContext.is_initialized() 
					&& static_cast<BerkeleyTransaction&>(transactionContext.get()).isActive()
						? static_cast<BerkeleyTransaction&>(transactionContext.get()).transaction
						: NULL; }
			
		private:
			// The Berkeley DB transaction that backs this class
			DbTxn* transaction;

			// Used for thread safety within critical sectinos
			boost::mutex synchronization;

			// Helper method to determine if this transaction remains active
			const bool isActive() const { return transaction != NULL; }
		};
	}
}
}
}

#endif