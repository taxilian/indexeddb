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
#include <AutoPtr.h>
#include <BrowserHostWrapper.h>
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

class CursorSync;
class IndexSync;
class DatabaseSync;
class KeyRange;

///<summary>
/// This class represents a synchronized object store per the Indexed Database API draft
///</summary>
class ObjectStoreSync : public ObjectStore,
						public Support::LifeCycleObservable<ObjectStoreSync>
	{
	public:
		// Create an object store, with or without a key path
		ObjectStoreSync(FB::BrowserHost host, DatabaseSync& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const std::string& keyPath, const bool autoIncrement);
		ObjectStoreSync(FB::BrowserHost host, DatabaseSync& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const bool autoIncrement);
		// Open an object store in the given mode
		ObjectStoreSync(FB::BrowserHost host, DatabaseSync& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const Implementation::ObjectStore::Mode mode);
		// TODO: The subtle differences in constructors will be confusing to subsequent developers; change to static factory methods and make these protected
		
		~ObjectStoreSync(void);

		// Close this object store
		void close();

		// Get the value from the object store identified by the given key
		FB::variant get(FB::variant key);
		// Put a new key/value pair into the object store
		FB::variant put(FB::variant value, const FB::CatchAll& args);
		// Remove the key/value pair from the object store as identified by the given key
		FB::variant remove(FB::variant key);

		// Open a new cursor over this object store, bounded by the given range
		FB::AutoPtr<CursorSync> openCursor(const boost::optional<KeyRange> range, const Cursor::Direction direction);

		// Create a new index over this object store
		FB::AutoPtr<IndexSync> createIndex(const std::string name, const boost::optional<std::string> keyPath, const bool unique); //raises (DatabaseException);
		// Remove an existing index from the object store
		void removeIndex(const std::string& indexName);
		void removeIndex(const Index& index) { removeIndex(index.getName()); }

		Implementation::ObjectStore& getImplementation() const { return *implementation; }

		virtual StringVector getIndexNames() const;
		virtual FB::variant getKeyPath() const { 
			return keyPath.is_initialized() ? keyPath.get() : FB::variant(); }

	protected:
		long nextKey;
		bool autoIncrement;
		boost::optional<std::string> keyPath;
		Support::Container<IndexSync> openIndexes;
		Support::Container<CursorSync> openCursors;

		boost::mutex synchronization;

		FB::BrowserHost host;
		FB::variant generateKey(FB::variant value);

		virtual void onTransactionAborted(const Transaction& transaction);
		virtual void onTransactionCommitted(const Transaction& transaction);

	private:
		const std::auto_ptr<Implementation::ObjectStore> implementation;
		Metadata metadata;
		TransactionFactory transactionFactory;

		FB::JSOutObject openCursor(const FB::CatchAll& args);
		FB::JSOutObject openIndex(const std::string& name);
		FB::JSOutObject createIndex(const std::string name, const FB::CatchAll& args);

		void initializeMethods();
		void createMetadata(const boost::optional<std::string>& keyPath, const bool autoIncrement, Implementation::TransactionContext& transactionContext);
		void loadMetadata(Implementation::TransactionContext& transactionContext);
	};

typedef boost::optional<std::list<FB::AutoPtr<ObjectStoreSync>>> ObjectStoreSyncList;
}
}
}

#endif