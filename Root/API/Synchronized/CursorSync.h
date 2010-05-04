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

class ObjectStoreSync;
class IndexSync;
class KeyRange;

class CursorSync : public Cursor, public Support::LifeCycleObservable<CursorSync>
	{
	public:
		CursorSync(FB::BrowserHost host, ObjectStoreSync& objectStore, TransactionFactory& transactionFactory, const boost::optional<KeyRange>& range, const Cursor::Direction direction);
		CursorSync(FB::BrowserHost host, IndexSync& index, TransactionFactory& transactionFactory, const boost::optional<KeyRange>& range, const Cursor::Direction direction, const bool returnKeys);
		virtual ~CursorSync(void);

		FB::variant getKey();
		FB::variant getValue();
		int getCount();
		bool next(const FB::CatchAll& args);
		FB::variant remove();
		int close();

	protected:
		virtual void onTransactionAborted(const Transaction& transaction) { }
		virtual void onTransactionCommitted(const Transaction& transaction) { }

	private:
		const std::auto_ptr<Implementation::Cursor> implementation;
		FB::BrowserHost host;
		TransactionFactory transactionFactory;

		const boost::optional<KeyRange> range;
		const bool readOnly;

		void initializeMethods();
	};

}
}
}

#endif