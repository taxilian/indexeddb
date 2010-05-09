/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

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

		///<summary>
		/// This abstract class represents a cursor in a Berkeley DB environment.
		/// In addition to providing default implementations for most methods
		/// required by the Cursor interface, this class provides helper functions
		/// useful in derived classes.
		///</summary>
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
			/// Construct a Berkeley DB-backed cursor with the given (left, right) interval (possibly open on one or both ends)
			BerkeleyCursor(Db& source, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext);

			boost::mutex synchronization;

			/// Initializes a cursor in the given transaction context; if none exists, it creates an implicit
			/// context (via implicitTransaction)
			Dbc* makeCursor(Db& source, TransactionContext transactionContext);
			Dbc* getCursor() { return cursor; }
			/// Helper method to iterate the underlying cursor
			virtual bool next(Dbc* cursor, TransactionContext& transactionContext);
			/// Helper method to ensure that the cursor is open; throw otherwise			
			void ensureOpen();

		private:
			// The implict transaction associated with this cursor (none if the cursor was created using an explicit context)
			DbTxn* implicitTransaction;
			Dbc* cursor;
			long totalCount;
			bool isOpen;

			/// Utility methods used to initialize a cursor with one of the (many) supported intervals 
			void startCursor();
			static void startCursor(BerkeleyCursor& berkeleyCursor);
			static void startCursor(BerkeleyCursor& berkeleyCursor, Dbc* cursor);
			static void startClosedLeftCursor(Dbc* cursor, const Key& left);
			static void startOpenLeftIntervalCursor(Dbc* cursor, const Key& left);
			static void startClosedRightReverseCursor(Dbc* cursor, const Key& right);
			static void startOpenRightIntervalReverseCursor(Dbc* cursor, const Key& right);
			static void closeCursor(Dbc* cursor);

			/// Utility method to determine if the current cursor is outside of its defined interval
			bool isOutOfRange(const Dbt& key);
			bool isOutOfRange(const Key& key);
		};
	}
}
}
}

#endif