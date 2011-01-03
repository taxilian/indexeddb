/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_OBJECTSTORESYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_OBJECTSTORESYNC_H

#include <string>
#include <list>
#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include <BrowserHost.h>
#include "IndexSync.h"
#include "CursorSync.h"
#include "../ObjectStore.h"
#include "../../Support/TransactionFactory.h"
#include "../../Support/Container.h"
#include "../../Support/Metadata.h"
#include "../../Support/LifeCycleObservable.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

FB_FORWARD_PTR(DatabaseSync);
FB_FORWARD_PTR(ObjectStoreSync);
FB_FORWARD_PTR(KeyRange);
FB_FORWARD_PTR(CursorSync);
FB_FORWARD_PTR(IndexSync);

///<summary>
/// This class represents a synchronized object store per the Indexed Database API draft
///</summary>
class ObjectStoreSync : public ObjectStore
	{
    protected:
        boost::shared_ptr<Support::LifeCycleObservable<ObjectStoreSync> > _observable;

	public: 
		// Create an object store, with or without a key path
		ObjectStoreSync(const FB::BrowserHostPtr& host, const DatabaseSyncPtr& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const std::string& keyPath, const bool autoIncrement);
		ObjectStoreSync(const FB::BrowserHostPtr& host, const DatabaseSyncPtr& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const bool autoIncrement);
		// Open an object store in the given mode
		ObjectStoreSync(const FB::BrowserHostPtr& host, const DatabaseSyncPtr& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const Implementation::ObjectStore::Mode mode);
		// TODO: The subtle differences in constructors will be confusing to subsequent developers; change to static factory methods and make these protected
		
		~ObjectStoreSync(void);

		// Close this object store
		void close();

		// Get the value from the object store identified by the given key
		FB::variant get(FB::variant key);
		// Put a new key/value pair into the object store
        FB::variant put(const FB::variant& value, const FB::variant& inKey, const boost::optional<bool> no_overwrite);
		// Remove the key/value pair from the object store as identified by the given key
		void remove(FB::variant key);

		// Open a new cursor over this object store, bounded by the given range
		boost::shared_ptr<CursorSync> openCursorDirect(const KeyRangePtr& range, const Cursor::Direction direction);

		// Create a new index over this object store
        IndexSyncPtr createIndex(const std::string name, const boost::optional<std::string> keyPath, const boost::optional<bool> in_unique);
		// Remove an existing index from the object store
		void removeIndex(const std::string& indexName);
		void removeIndex(const Index& index);

		// Many objects in the synchronized family need access to an object store's implementation; we expose it broadly
		// instead of friending ~5 classes/methods.
		Implementation::ObjectStore& getImplementation() const;

		// Get the names of the indexes associated with this object store
		virtual StringVector getIndexNames() const;
		// Get this object's key path (if any; undefined otherwise)
		virtual FB::variant getKeyPath() const;

	protected:
		long nextKey;
		bool autoIncrement;
		boost::optional<std::string> keyPath;
		// We maintain a set of indexes and cursors that have been opened under this object store
		boost::shared_ptr<Support::Container<IndexSync> > openIndexes;
		boost::shared_ptr<Support::Container<CursorSync> > openCursors;

		// Some of our operations are not thread-safe, so we synchronize
		boost::mutex synchronization;

		FB::BrowserHostPtr host;
		// For object stores that automatically generate keys, this method does so
		// TODO: This should be replaced by a key store
		FB::variant generateKey(FB::variant value);

    public:
        typedef boost::shared_ptr<Support::LifeCycleObserver<ObjectStoreSync> > LifeCycleObserverPtr;
        // Forwarding methods for the embedded Observable
		void addLifeCycleObserver(const LifeCycleObserverPtr& observer);
		void removeLifeCycleObserver(const LifeCycleObserverPtr& observer);
		// This class receives messages that notify of committed and aborted transactions within the database scope
		virtual void onTransactionAborted(const TransactionPtr& transaction);
		virtual void onTransactionCommitted(const TransactionPtr& transaction);

	private:
		// We own a pointer to our implementation
		const std::auto_ptr<Implementation::ObjectStore> implementation;
		// We maintain a reference to the database's metabase
		Metadata metadata;
		// We need a transaction factory to get current transaction context and initiate new transactions
		TransactionFactory transactionFactory;

		// Internal operations to expose our functionality as weakly-typed methods to user agents
        CursorSyncPtr openCursor(const boost::optional<FB::VariantMap> info, const boost::optional<int> dir);
		IndexSyncPtr openIndex(const std::string& name);

		void initializeMethods();

		// When an object store is opened or updated, we'll need to create/update the metadata associated therewith
		void createMetadata(const boost::optional<std::string>& keyPath, const bool autoIncrement, Implementation::TransactionContext& transactionContext);
		void loadMetadata(Implementation::TransactionContext& transactionContext);
	};

// This typedef represents an optional list of synchronized object stores.  It is used to translate
// a set of object stores into implementations (see TransactionSync) during static transactions.
typedef boost::optional<std::list<boost::shared_ptr<ObjectStoreSync>>> ObjectStoreSyncList;
}
}
}

#endif