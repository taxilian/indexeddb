/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_SUPPORT_TRANSACTIONFACTORY_H
#define BRANDONHAYNES_INDEXEDDB_SUPPORT_TRANSACTIONFACTORY_H

#include <vector>
#include <string>
#include "../Implementation/Transaction.h"
#include "../Implementation/AbstractDatabaseFactory.h"

typedef std::vector<std::string> StringVector;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class represents a source for (i) the current transaction context, (ii) the current database context,
/// and (iii) a factory for new transactions.
///</summary>
// TODO: We might be violating the SRP here; should we separate?  If not, this class could use a better name
// TODO: Chaining our calls is a bit silly.  Pass references (see RootTransactionFactory).
class TransactionFactory
	{
	public:
		TransactionFactory(TransactionFactory& transactionFactory) 
			: transactionFactory(&transactionFactory)
			{ }

		// Defer all calls to our "parent" transaction factory, ultimately implemented by the RootTransactionFactory
		virtual Implementation::TransactionContext getTransactionContext() const
			{ return transactionFactory->getTransactionContext(); }
		virtual Implementation::Database& getDatabaseContext() const
			{ return transactionFactory->getDatabaseContext(); }

		std::auto_ptr<Implementation::Transaction> createTransaction() const
			{ return createTransaction(getTransactionContext()); }

		// We know how to initiate a new transaction, so do it here
		std::auto_ptr<Implementation::Transaction> createTransaction(Implementation::TransactionContext& transactionContext) const
			{ return Implementation::AbstractDatabaseFactory::getInstance()
				.createTransaction(getDatabaseContext(), 
					Implementation::ObjectStoreImplementationList(), 
					boost::optional<unsigned int>(), 
					transactionContext); }

	protected:
		TransactionFactory() { }

	private:
		const TransactionFactory* transactionFactory;
	};

}
}
}

#endif