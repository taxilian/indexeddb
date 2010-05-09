/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_CURSOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_CURSOR_H

#include "Transaction.h"
#include "Key.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Data;

	///<summary>
	/// This class represents the implementation of a cursor.  Specific persistence providers implement it
	/// in a manner appropriate to that domain.
	///</summary>
	class Cursor
		{
		public:
			virtual ~Cursor() { }

			// Gets the key associated with the current cursor position
			virtual Key getKey() = 0;
			// Gets the data associated with the current cursor position
			virtual Data getData(TransactionContext& transactionContext) = 0;
			// Gets the number of key/value pairs associated with this cursor (over the given interval0
			virtual unsigned long getCount(TransactionContext& transactionContext) = 0;
			// Iterates the cursor to the next key/value pair
			virtual bool next(TransactionContext& transactionContext) = 0;
			// Moves the cursor to the first key/value pair that has primary key "key"
			virtual bool next(const Key& key) = 0;
			// Removes the key/value pair at the current cursor position
			virtual void remove() = 0;
			// Closes the cursor.  Subsequent operations on this cursor may throw.
			virtual void close() = 0;

		protected:
			// Creates a cursor with the given interval and in the given direction
			Cursor(const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates)
				: left(Key(left)), right(Key(right)), 
				  openLeft(openLeft), openRight(openRight),
				  isReversed(isReversed), omitDuplicates(omitDuplicates)
				{ }

			// The left bound for this cursor
			const Key left;
			// The right bound for this interval
			const Key right;
			// Flag indicating whether the left bound for this interval is open
			const bool openLeft;
			// Flag indicating whether the right bound for this interval is open
			const bool openRight;
			// Flag indicating whether the direction for this cursor is forward (false) or reversed (true)
			const bool isReversed;
			// Flag indicating whether duplicate keys should be skipped during iteration
			const bool omitDuplicates;
		};
	}
}
}

#endif