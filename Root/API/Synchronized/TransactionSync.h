/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_TRANSACTIONSYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_TRANSACTIONSYNC_H

#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include "../Transaction.h"
#include "ObjectStoreSync.h"
#include "../../Support/RootTransactionFactory.h"
#include "../../Support/TransactionFactory.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class represents a synchronized transaction in the Indexed Database API.
///</summary>
class TransactionSync : public Transaction
{
public:
	TransactionSync(DatabaseSync& database, TransactionFactory& transactionFactory, const ObjectStoreSyncList& objectStores, const boost::optional<unsigned int>& timeout);
	~TransactionSync();

	bool getIsActive() const { return isActive; }

	// Not much to do here; transactions do one of two things...
	virtual void abort();
    virtual void commit();

	// Forcably close this transaction (technically behavior is undefined, but we all know this will abort...)
	void close();

protected:
	// Get the underlying implementation associated with this transaction (which is itself a TransactionContext)
	virtual Implementation::TransactionContext getTransactionContext() const { return *implementation; }

private:
	// Own a reference to our underlying implementation
	std::auto_ptr<Implementation::Transaction> implementation;
	// Maintain a transaction factory so we can initiate and manage transactions
	//TransactionFactory transactionFactory;
	bool isActive;

	boost::mutex synchronization;

	static const Implementation::ObjectStoreImplementationList
		mapObjectStoresToImplementations(const ObjectStoreSyncList& objectStores);

	struct MapObjectStoreToImplementationFunctor
		{
		MapObjectStoreToImplementationFunctor(Implementation::ObjectStoreImplementationList& objectStoreImplementations) 
			: objectStoreImplementations(objectStoreImplementations)
			{ if(!objectStoreImplementations.is_initialized()) objectStoreImplementations = std::list<Implementation::ObjectStore*>(); }
		void operator()(const FB::AutoPtr<ObjectStoreSync>& objectStore)
			{ objectStoreImplementations.get().push_back(&objectStore->getImplementation()); }

		Implementation::ObjectStoreImplementationList& objectStoreImplementations;
		};

	friend Implementation::TransactionContext RootTransactionFactory::getTransactionContext() const;
};

}
}
}

#endif