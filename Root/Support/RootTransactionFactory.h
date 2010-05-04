/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_ROOTTRANSACTIONFACTORY_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_ROOTTRANSACTIONFACTORY_H

#include "TransactionFactory.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

	class DatabaseSync;
	class Implementation::Database;

	class RootTransactionFactory : public TransactionFactory
		{
		public:
			virtual Implementation::TransactionContext getTransactionContext() const;
			virtual Implementation::Database& getDatabaseContext() const
				{ return implementation; }

			RootTransactionFactory(DatabaseSync& database, Implementation::Database& implementation) 
				: database(database), implementation(implementation), TransactionFactory()
				{ }

		private:
			DatabaseSync& database;
			Implementation::Database& implementation;
		};
	}
}
}

#endif