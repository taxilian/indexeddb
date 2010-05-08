/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_INDEXSYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_INDEXSYNC_H

#include <string>
#include <boost/optional.hpp>
#include "../Index.h"
#include "../Cursor.h"
#include "../../Support/Metadata.h"
#include "../../Support/Container.h"
#include "../../Support/TransactionFactory.h"
#include "../../Support/LifeCycleObservable.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class ObjectStoreSync;
class KeyRange;
class CursorSync;

class IndexSync : public Index, public Support::LifeCycleObservable<IndexSync>
{
public:
	IndexSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const std::string& name);
	IndexSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const std::string& name, const boost::optional<std::string>& keyPath, const bool unique);
	~IndexSync(void);

    FB::variant get(FB::variant key);
	FB::variant getObject(FB::variant key);
	FB::variant put(FB::variant value, const FB::CatchAll& args);
	FB::variant remove(FB::variant key);
	void close();

	FB::AutoPtr<CursorSync> openCursor(const boost::optional<KeyRange>& range, const Cursor::Direction direction, const bool dataArePrimaryKeys);

protected:
	Support::Container<CursorSync> openCursors;

	virtual void onTransactionAborted(const Transaction& transaction);
	virtual void onTransactionCommitted(const Transaction& transaction);

private:
	FB::BrowserHost host;
	std::auto_ptr<Implementation::KeyGenerator> keyGenerator;
	std::auto_ptr<Implementation::Index> implementation;
	Metadata metadata;
	TransactionFactory transactionFactory;

	FB::JSOutObject openCursor(const FB::CatchAll& args);
	FB::JSOutObject openObjectCursor(const FB::CatchAll& args);

	void initializeMethods();
	void createMetadata(const boost::optional<std::string>& keyPath, const bool unique);
	void loadMetadata();

	friend class CursorSync;
};

}
}
}

#endif