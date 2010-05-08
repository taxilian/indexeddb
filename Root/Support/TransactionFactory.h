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

class TransactionFactory
	{
	public:
		TransactionFactory(TransactionFactory& transactionFactory) 
			: transactionFactory(&transactionFactory)
			{ }

		virtual Implementation::TransactionContext getTransactionContext() const
			{ return transactionFactory->getTransactionContext(); }
		virtual Implementation::Database& getDatabaseContext() const
			{ return transactionFactory->getDatabaseContext(); }

		std::auto_ptr<Implementation::Transaction> createTransaction() const
			{ return createTransaction(getTransactionContext()); }

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