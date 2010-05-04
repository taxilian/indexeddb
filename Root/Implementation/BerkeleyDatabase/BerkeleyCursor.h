#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYCURSOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYCURSOR_H

#include <string>
#include <boost/thread/mutex.hpp>
#include "../Cursor.h"
#include "../Key.h"
#include "../Data.h"

class Db;
class Dbc;
class DbTxn;
class Dbt;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Key;
	class Data;
		
	namespace BerkeleyDB {

		class BerkeleyCursor : public Cursor
		{
		public:
			virtual ~BerkeleyCursor();

			virtual Key getKey();
			virtual Data getData(TransactionContext& transactionContext);
			virtual unsigned long getCount(TransactionContext& transactionContext);
			virtual bool next(TransactionContext& transactionContext);
			virtual bool next(const Key& key);
			virtual void close();
			virtual void remove();

		protected:
			BerkeleyCursor(Db& source, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext);

			boost::mutex synchronization;

			Dbc* makeCursor(Db& source, TransactionContext transactionContext);
			Dbc* getCursor() { return cursor; }
			virtual bool next(Dbc* cursor, TransactionContext& transactionContext);
			void ensureOpen();

		private:
			DbTxn* implicitTransaction;
			Dbc* cursor;
			long totalCount;
			bool isOpen;

			void startCursor();
			static void startCursor(BerkeleyCursor& berkeleyCursor);
			static void startCursor(BerkeleyCursor& berkeleyCursor, Dbc* cursor);
			static void startClosedLeftCursor(Dbc* cursor, const Key& left);
			static void startOpenLeftIntervalCursor(Dbc* cursor, const Key& left);
			static void startClosedRightReverseCursor(Dbc* cursor, const Key& right);
			static void startOpenRightIntervalReverseCursor(Dbc* cursor, const Key& right);
			static void closeCursor(Dbc* cursor);

			bool isOutOfRange(const Dbt& key);
			bool isOutOfRange(const Key& key);
		};
	}
}
}
}

#endif