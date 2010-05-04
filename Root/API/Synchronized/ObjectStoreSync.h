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

class ObjectStoreSync : public ObjectStore,
						public Support::LifeCycleObservable<ObjectStoreSync>
	{
	public:
		ObjectStoreSync(FB::BrowserHost host, DatabaseSync& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const std::string& keyPath, const bool autoIncrement);
		ObjectStoreSync(FB::BrowserHost host, DatabaseSync& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const bool autoIncrement);
		ObjectStoreSync(FB::BrowserHost host, DatabaseSync& database, TransactionFactory& transactionFactory, Implementation::TransactionContext& transactionContext, Metadata& metadata, const std::string& name, const Implementation::ObjectStore::Mode mode);
		~ObjectStoreSync(void);

		void close();

		FB::variant get(FB::variant key);
		FB::variant put(FB::variant value, const FB::CatchAll& args);
		FB::variant remove(FB::variant key);

		FB::AutoPtr<CursorSync> openCursor(const boost::optional<KeyRange> range, const Cursor::Direction direction);

		FB::AutoPtr<IndexSync> createIndex(const std::string name, const boost::optional<std::string> keyPath, const bool unique); //raises (DatabaseException);
		long removeIndex(const std::string& indexName);

		Implementation::ObjectStore& getImplementation() const { return *implementation; }

		virtual StringVector getIndexNames() const;
		virtual const FB::variant getKeyPath() const { 
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