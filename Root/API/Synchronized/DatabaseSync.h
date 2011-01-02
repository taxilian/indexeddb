/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_DATABASESYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_DATABASESYNC_H

#include <boost/optional.hpp>
#include <APITypes.h>
#include "../Database.h"
#include "../../Support/Metadata.h"
#include "../../Support/Container.h"
#include "../../Support/RootTransactionFactory.h"
#include "ObjectStoreSync.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class TransactionSync;

///<summary>
/// This class represents a synchronized database in the Indexed Database API
///</summary>
class DatabaseSync;
typedef boost::shared_ptr<DatabaseSync> DatabaseSyncPtr;

class DatabaseSync : public Database
	{
    protected:
		DatabaseSync(FB::BrowserHostPtr host, const std::string& name, const std::string& description, const bool modifyDatabase);
        void init();

	public:
		static DatabaseSyncPtr create(FB::BrowserHostPtr host, const std::string& name, const std::string& description, const bool modifyDatabase);
		// Creates or opens a synchronized database with the given attributes
		~DatabaseSync();

		// Gets the origin of this database
		std::string getOrigin();

		virtual TransactionPtr getCurrentTransaction() const;

		virtual std::vector<std::string> getObjectStoreNames()
			{ return metadata.getMetadataCollection("objectStores", transactionFactory.getTransactionContext()); }

		// Creates or opens an object store within this database
		boost::shared_ptr<ObjectStoreSync> createObjectStore(const std::string& name, const bool autoIncrement);
		boost::shared_ptr<ObjectStoreSync> createObjectStore(const std::string& name, const std::string& keyPath, const bool autoIncrement);
		boost::shared_ptr<ObjectStoreSync> openObjectStore(const std::string& name, Implementation::ObjectStore::Mode mode);

		// Removes an object store with the given name
		long removeObjectStore(const std::string& storeName);

		// Gets the set of index names associated with this database
		virtual StringVector getIndexNames();

		// Manipulate this database's version
		virtual boost::optional<std::string> getVersion() const;
		virtual void setVersion(const std::string& version);
	    
		// Initiate a transaction on this database (the API supports exactly one such transaction at a time)
		boost::shared_ptr<TransactionSync> transaction(const StringVector& inStoreNames, const boost::optional<unsigned int>& timeout);

	protected:
		// This is undefined in the spec, but required
		const bool modifyDatabase;
		FB::BrowserHostPtr host;

		// These methods convert incoming user agent requests (via FireBreath) into strongly-typed calls
		FB::JSAPIPtr createObjectStore(const std::string& name, const FB::CatchAll& args);
		FB::JSAPIPtr openObjectStore(const std::string& name, const FB::CatchAll& args);
		boost::shared_ptr<TransactionSync> transaction(const std::string& objectStoreName, const boost::optional<unsigned int>& timeout);

		FB::JSAPIPtr transaction(const FB::CatchAll& args);

		// This method converts the optional transaction field into a FireBreath-compatible variant
		virtual FB::variant getCurrentTransactionVariant() const;

		// We need to be notified if a transaction is aborted or committed, so we can clear our current transaction
		virtual void onTransactionAborted(const TransactionPtr& transaction);
		virtual void onTransactionCommitted(const TransactionPtr& transaction);

	protected:
		// We maintain a weak list of object stores, mostly to ensure they're closed before we close this instance
		Support::Container<ObjectStoreSync> openObjectStores;

	private:
		// Maintain a reference to our underlying implementation
		const std::auto_ptr<Implementation::Database> implementation;
		// Maintain a shared pointer to our current transaction
		boost::shared_ptr<TransactionSync> currentTransaction;
		// Maintain a reference to a transaction factory, to get current context and initiate new transactions
		RootTransactionFactory transactionFactory;
		// Maintain a reference to our underlying metadata store
		Metadata metadata;

		// Helper method to ensure that we're actually allowed to create the named object store
		void ensureCanCreateObjectStore(const std::string& name);

		// Functor to map names to ObjectStoreSync instances; used to initiate a static transaction
		struct MapObjectStoreNameToObjectStoreFunctor : public std::unary_function<void, const std::string&>
			{
			MapObjectStoreNameToObjectStoreFunctor(const DatabaseSyncPtr& database, ObjectStoreSyncList& objectStores)
				: database(database), objectStores(objectStores)
				{ if(!objectStores.is_initialized()) objectStores = std::list<boost::shared_ptr<ObjectStoreSync>>(); }
			void operator()(const std::string& objectStoreName)
				//TODO we really want to be using an internal map of object stores, not opening anew; will have ownership issues here though
				{ objectStores.get().push_back(database->openObjectStore(objectStoreName, Implementation::ObjectStore::READ_WRITE)); }

			const DatabaseSyncPtr& database;
			ObjectStoreSyncList& objectStores;
			};

		friend Implementation::TransactionContext RootTransactionFactory::getTransactionContext() const;
        friend class Support::Container<ObjectStoreSync>;
	};

}
}
}

#endif