/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#include "BerkeleyIndexCursor.h"
#include "BerkeleyIndex.h"
#include "BerkeleyDatabase.h"
#include "../ImplementationException.h"

using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyIndexCursor::BerkeleyIndexCursor(BerkeleyIndex& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool dataArePrimaryKeys, TransactionContext& transactionContext)
		: BerkeleyCursor(index.implementation, left, right, openLeft, openRight, isReversed, omitDuplicates, transactionContext),
		  dataArePrimaryKeys(dataArePrimaryKeys)
		{ }

	Data BerkeleyIndexCursor::getData(TransactionContext& transactionContext)
		{
		ensureOpen();

		if(!dataArePrimaryKeys)
			return BerkeleyCursor::getData(transactionContext);
		else
			{
			Dbt key, primaryKey, data;

			try
				{
				if(getCursor()->pget(&key, &primaryKey, &data, DB_CURRENT) == 0)
						return BerkeleyDatabase::ToData(primaryKey);
					else
						return Data::getUndefinedData();
				}
			catch(DbDeadlockException& e)
				{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
			catch(DbException& e)
				{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
			}
		}
	}
}
}
}