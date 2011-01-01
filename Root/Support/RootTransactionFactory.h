/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_ROOTTRANSACTIONFACTORY_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_ROOTTRANSACTIONFACTORY_H

#include "TransactionFactory.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

	class DatabaseSync;
    typedef boost::shared_ptr<DatabaseSync> DatabaseSyncPtr;
	class Implementation::Database;

	///<summary>
	/// This class represents the root of the transaction factory tree.  Most all synchronized
	/// classes hold on to a reference of the TransactionFactory, but defer to this one for
	/// actual implementation.
	///</summary>
	// TODO: This is a little silly; we should be passing interface references and not chaining calls.
	class RootTransactionFactory : public TransactionFactory
		{
		public:
			virtual Implementation::TransactionContext getTransactionContext() const;
			virtual Implementation::Database& getDatabaseContext() const
				{ return implementation; }

			// Associate a new transaction factory with the given database and underlying implementation
			RootTransactionFactory(Implementation::Database& implementation) 
				: implementation(implementation), TransactionFactory()
				{ }
            void setDatabaseSync( const DatabaseSyncPtr& ptr );
		private:
			DatabaseSyncPtr database;
			Implementation::Database& implementation;
		};
	}
}
}

#endif