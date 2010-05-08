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

class TransactionSync : public Transaction
{
public:
	TransactionSync(DatabaseSync& database, TransactionFactory& transactionFactory, const ObjectStoreSyncList& objectStores, const boost::optional<unsigned int>& timeout);
	~TransactionSync();

	bool getIsActive() const { return isActive; }

	virtual long abort();
    virtual long commit();
	void close();

protected:
	virtual Implementation::TransactionContext getTransactionContext() const { return *implementation; }

private:
	std::auto_ptr<Implementation::Transaction> implementation;
	TransactionFactory transactionFactory;
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