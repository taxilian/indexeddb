/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "BerkeleyManualIndexCursor.h"
#include "BerkeleyManualIndex.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyObjectStore.h"
#include "../ImplementationException.h"

using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyManualIndexCursor::BerkeleyManualIndexCursor(BerkeleyManualIndex& index, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, const bool dataArePrimaryKeys, TransactionContext& transactionContext)
		: BerkeleyCursor(index.implementation, left, right, openLeft, openRight, isReversed, omitDuplicates, transactionContext),
		  index(index),
		  dataArePrimaryKeys(dataArePrimaryKeys),
		  currentValue(Data::getUndefinedData())
		{ 
		ensurePrimaryKeyExists(transactionContext);
		}

	Data BerkeleyManualIndexCursor::getData(TransactionContext& transactionContext)
		{ 
		ensureOpen();
		return currentValue; 
		}

	bool BerkeleyManualIndexCursor::next(Dbc* cursor, TransactionContext& transactionContext)
		{
		return BerkeleyCursor::next(cursor, transactionContext) && ensurePrimaryKeyExists(transactionContext);
		}

	bool BerkeleyManualIndexCursor::ensurePrimaryKeyExists(TransactionContext& transactionContext)
		{
		Dbt secondaryKey, primaryKeyDbt;
		Data result = Data::getUndefinedData();

		ensureOpen();

		try
			{
			if(getCursor()->get(&secondaryKey, &primaryKeyDbt, DB_CURRENT) == 0)
				{
				Key& primaryKey(BerkeleyDatabase::ToKey(primaryKeyDbt));
				if(!index.objectStore.exists(primaryKey, transactionContext))
					{
					index.remove(BerkeleyDatabase::ToKey(secondaryKey), transactionContext);
					return next(getCursor(), transactionContext);
					}
				else
					currentValue = 
						(dataArePrimaryKeys || primaryKey == Data::getUndefinedData()
							? primaryKey
							: index.objectStore.get(primaryKey, transactionContext));
				}
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }

		return true;
		}

	void BerkeleyManualIndexCursor::remove()
		{
		currentValue = Data::getUndefinedData();
		BerkeleyCursor::remove();
		}
	}	
}
}
}