/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_TRANSACTION_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_TRANSACTION_H

namespace boost { template<class T> class optional; }

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Database;

	///<summary>
	/// This class represents the implementation of a transaction in the Indexed Database API.
	///</summary>
	class Transaction
		{
		public:
			// Not much to do here.  We commit or abort.
			virtual void commit() = 0;
			virtual void abort() = 0;
		};

	// This typedef represents a transaction context (wow, really?).  All transactional operations require a context,
	// which means that there is no active transaction (optional::is_initialized = false) or there IS an active
	// transaction (*optional == the transaction).  This allows operations that require nested transactions (e.g.
	// metadata updates) to easily get the current transaction state and nest if necessary.
	typedef const boost::optional<Transaction&> TransactionContext;
	}
}
}

#endif