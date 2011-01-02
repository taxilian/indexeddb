/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_CURSORSYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_CURSORSYNC_H

#include <boost/optional.hpp>
#include "../Cursor.h"
#include "../KeyRange.h"
#include "../../Support/TransactionFactory.h"
#include "../../Support/LifeCycleObservable.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

FB_FORWARD_PTR(ObjectStoreSync);
FB_FORWARD_PTR(IndexSync);
FB_FORWARD_PTR(KeyRange);

///<summary>
/// This class represents a synchronized cursor as defined by the Indexed Database API.
///</summary>
class CursorSync : public Cursor
	{
    protected:
        typedef boost::shared_ptr<Support::LifeCycleObserver<CursorSync> > LifeCycleObserverPtr;
        boost::shared_ptr<Support::LifeCycleObservable<CursorSync> > _observable;
	public:
		// This constructor creates a cursor over the given object store
		CursorSync(FB::BrowserHostPtr host, const ObjectStoreSyncPtr& objectStore, TransactionFactory& transactionFactory, const KeyRangePtr& range, const Cursor::Direction direction);
		// This constructor creates a cursor over the given index
		CursorSync(FB::BrowserHostPtr host, const IndexSyncPtr& index, TransactionFactory& transactionFactory, const KeyRangePtr& range, const Cursor::Direction direction, const bool returnKeys);
		virtual ~CursorSync(void);

		// Gets the key associated with the current cursor position
		FB::variant getKey();
		// Gets the value associated with the current cursor position
		FB::variant getValue();
		// Gets the count of key/value pairs associated with this cursor, over the configured key range
		int getCount();
		// Iterates the cursor either to the next key/value pair, or the given (optional) key
		bool next(const FB::CatchAll& args);
		// Remove the key/value pair at the current cursor position
		void remove();
		// Close this cursor
		void close();

	public:
        // Forwarding methods for the embedded Observable
		void addLifeCycleObserver(const LifeCycleObserverPtr& observer);
		void removeLifeCycleObserver(const LifeCycleObserverPtr& observer);
		// By being lifecycle-observable, this object will receive messages when an associated transaction is 
		// aborted or committed.  We don't need any of that implementation here.
		virtual void onTransactionAborted(const TransactionPtr& transaction) { }
		virtual void onTransactionCommitted(const TransactionPtr& transaction) { }

	private:
		// The underlying implementation for this cursor
		const std::auto_ptr<Implementation::Cursor> implementation;
		// We need a reference to the browser for JSON encoding
		FB::BrowserHostPtr host;
		// Hold a reference to a factory for transactions (including for our current context)
		TransactionFactory transactionFactory;

		// Cursors may or may not have an associated range
		const KeyRangePtr range;
		const bool readOnly;

		void initializeMethods();
};

}
}
}

#endif