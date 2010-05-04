/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_DATABASESYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_DATABASESYNC_H

#include <boost/optional.hpp>
#include <APITypes.h>
#include <Util/JSArray.h>
#include "../Database.h"
#include "../../Support/Metadata.h"
#include "../../Support/Container.h"
#include "../../Support/RootTransactionFactory.h"
#include "ObjectStoreSync.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class TransactionSync;

class DatabaseSync : public Database
	{
	public:
		DatabaseSync(FB::BrowserHost host, const std::string& name, const std::string& description, const bool modifyDatabase);
		~DatabaseSync();

		const std::string getOrigin();

		virtual boost::optional<TransactionSync&> getCurrentTransaction() const;

		virtual std::vector<std::string> getObjectStoreNames()
			{ return metadata.getMetadataCollection("objectStores", transactionFactory.getTransactionContext()); }

		FB::AutoPtr<ObjectStoreSync> createObjectStore(const std::string& name, const bool autoIncrement);
		FB::AutoPtr<ObjectStoreSync> createObjectStore(const std::string& name, const std::string& keyPath, const bool autoIncrement);
		FB::AutoPtr<ObjectStoreSync> openObjectStore(const std::string& name, Implementation::ObjectStore::Mode mode);

		long removeObjectStore(const std::string& storeName);

		virtual StringVector getIndexNames();

		virtual FB::variant getVersion() const;
		virtual long setVersion(const std::string& version);
	    
		FB::AutoPtr<TransactionSync> transaction(boost::optional<StringVector>& storeNames, const boost::optional<unsigned int>& timeout);

	protected:
		const bool modifyDatabase;
		FB::BrowserHost host;

		FB::JSOutObject createObjectStore(const std::string& name, const FB::CatchAll& args);
		FB::JSOutObject openObjectStore(const std::string& name, const FB::CatchAll& args);
		FB::AutoPtr<TransactionSync> transaction(const std::string& objectStoreName, const boost::optional<unsigned int>& timeout);
		FB::AutoPtr<TransactionSync> transaction(const FB::JSArray& objectStoreNames, const boost::optional<unsigned int>& timeout);

		FB::JSOutObject transaction(const FB::CatchAll& args);
		virtual FB::variant getCurrentTransactionVariant() const;

	protected:
		Support::Container<ObjectStoreSync> openObjectStores;

		virtual void onTransactionAborted(const Transaction& transaction);
		virtual void onTransactionCommitted(const Transaction& transaction);

	private:
		const std::auto_ptr<Implementation::Database> implementation;
		boost::optional<FB::AutoPtr<TransactionSync>> currentTransaction;
		RootTransactionFactory transactionFactory;
		Metadata metadata;

		void ensureCanCreateObjectStore(const std::string& name);

		struct MapObjectStoreNameToObjectStoreFunctor : public std::unary_function<void, const std::string&>
			{
			MapObjectStoreNameToObjectStoreFunctor(DatabaseSync& database, ObjectStoreSyncList& objectStores)
				: database(database), objectStores(objectStores)
				{ if(!objectStores.is_initialized()) objectStores = std::list<FB::AutoPtr<ObjectStoreSync>>(); }
			void operator()(const std::string& objectStoreName)
				//TODO we really want to be using an internal map of object stores, not opening anew; will have ownership issues here though
				{ objectStores.get().push_back(database.openObjectStore(objectStoreName, Implementation::ObjectStore::READ_WRITE)); }

			DatabaseSync& database;
			ObjectStoreSyncList& objectStores;
			};
	};

}
}
}

#endif