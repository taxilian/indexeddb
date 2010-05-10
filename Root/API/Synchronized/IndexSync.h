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

///<summary>
/// This class represents a synchronized index in the Indexed Database API.
///</summary>
class IndexSync : public Index, public Support::LifeCycleObservable<IndexSync>
{
public:
	IndexSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const std::string& name);
	IndexSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const std::string& name, const boost::optional<std::string>& keyPath, const bool unique);
	~IndexSync(void);

	// Get the primary key associated with the given secondary key in the index
    FB::variant get(FB::variant secondaryKey);
	// Get the primary value associated with the given secondary key in the index
	FB::variant getObject(FB::variant secondaryKey);
	// Update the primary value associated with the secondary key in the database
	FB::variant put(FB::variant value, const FB::CatchAll& args);
	// Remove the secondary key (and possibily the primary key/value pair) from the index
	void remove(FB::variant secondaryKey);
	// Close this index
	void close();

	// Open a cursor over this index
	FB::AutoPtr<CursorSync> openCursor(const boost::optional<KeyRange>& range, const Cursor::Direction direction, const bool dataArePrimaryKeys);

protected:
	// We maintain a set of opened cursors; these must be forcably closed prior to closing the index
	Support::Container<CursorSync> openCursors;

	// We need to be notified when a transaction commits so we can close our cursors
	virtual void onTransactionAborted(const Transaction& transaction);
	virtual void onTransactionCommitted(const Transaction& transaction);

private:
	FB::BrowserHost host;
	// We own a key generator for this index
	std::auto_ptr<Implementation::KeyGenerator> keyGenerator;
	// We own an underlying implementation for this index
	std::auto_ptr<Implementation::Index> implementation;
	// Maintain a reference to our database metadata; we include some of our immutable properties therein
	Metadata metadata;
	// Maintain a reference to a transaction factory, so we can initiate new transactions if necessary
	TransactionFactory transactionFactory;

	// Methods to interact between the user agent and this class; it interprets the args and calls the strongly typed overloads
	FB::JSOutObject openCursor(const FB::CatchAll& args);
	FB::JSOutObject openObjectCursor(const FB::CatchAll& args);

	void initializeMethods();

	// When an index is created or opened, we need to initialize our metadata; these methods do so
	void createMetadata(const boost::optional<std::string>& keyPath, const bool unique);
	void loadMetadata();

	// Cursors over indexes need to access some of our state
	friend class CursorSync;
};

}
}
}

#endif