/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_CURSOR_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_CURSOR_H

#include "Transaction.h"
#include "Key.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class Data;

	class Cursor
		{
		public:
			virtual ~Cursor() { }

			virtual Key getKey() = 0;
			virtual Data getData(TransactionContext& transactionContext) = 0;
			virtual unsigned long getCount(TransactionContext& transactionContext) = 0;
			virtual bool next(TransactionContext& transactionContext) = 0;
			virtual bool next(const Key& key) = 0;
			virtual void remove() = 0;
			virtual void close() = 0;

		protected:
			Cursor(const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates)
				: left(Key(left)), right(Key(right)), 
				  openLeft(openLeft), openRight(openRight),
				  isReversed(isReversed), omitDuplicates(omitDuplicates)
				{ }

			const Key left;
			const Key right;
			const bool openLeft;
			const bool openRight;
			const bool isReversed;
			const bool omitDuplicates;
		};
	}
}
}

#endif