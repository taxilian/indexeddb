/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_TRANSACTION_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_TRANSACTION_H

namespace boost { template<class T> class optional; }

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Database;

	class Transaction
		{
		public:
			virtual ~Transaction() { }

			virtual void commit() = 0;
			virtual void abort() = 0;
		};

	typedef const boost::optional<Transaction&> TransactionContext;
	}
}
}

#endif