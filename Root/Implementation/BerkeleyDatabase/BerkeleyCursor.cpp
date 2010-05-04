#include <atlstr.h>
#include <db_cxx.h>
#include "BerkeleyCursor.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyTransaction.h"
#include "..\Key.h"
#include "..\Data.h"
#include "..\ImplementationException.h"

using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyCursor::BerkeleyCursor(Db& source, const Key& left, const Key& right, const bool openLeft, const bool openRight, const bool isReversed, const bool omitDuplicates, TransactionContext& transactionContext)
		: Cursor(left, right, openLeft, openRight, isReversed, omitDuplicates),
		  cursor(makeCursor(source, transactionContext)),
		  isOpen(true)
		{
		try
			{
			totalCount = -1;
			startCursor();

			if(isOutOfRange(getKey()))
				throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR);
			}
		catch(ImplementationException&)
			{
			close();
			throw;
			}
		}

	BerkeleyCursor::~BerkeleyCursor()
		{ close(); }

	Key BerkeleyCursor::getKey()
		{
		Dbt key, data;
		int result;
		
		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if((result = cursor->get(&key, &data, DB_CURRENT)) == 0)
				return BerkeleyDatabase::ToKey(key);
			else if(result == DB_KEYEMPTY)
				return Key::getUndefinedKey();
			else
				throw ImplementationException("DATA_ERR", ImplementationException::DATA_ERR, result);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	Data BerkeleyCursor::getData(TransactionContext& transactionContext)
		{
		Dbt key, data;
		int result;

		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if((result = cursor->get(&key, &data, DB_CURRENT)) == 0)
				return BerkeleyDatabase::ToData(data);
			else if(result == DB_KEYEMPTY)
				return Key::getUndefinedKey();
			else
				throw ImplementationException("DATA_ERR", ImplementationException::DATA_ERR, result);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	bool BerkeleyCursor::next(TransactionContext& transactionContext)
		{ return next(cursor, transactionContext); }

	bool BerkeleyCursor::next(Dbc* cursor, TransactionContext& transactionContext)
		{
		Dbt key, data;

		ensureOpen();

		try
			{
			int result = cursor->get(&key, &data, isReversed 
				? (omitDuplicates ? DB_PREV_NODUP : DB_PREV)
				: (omitDuplicates ? DB_NEXT_NODUP : DB_NEXT));

			if(result == 0)
				return !isOutOfRange(key);
			else if(result == DB_NOTFOUND)
				return false;
			else
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	bool BerkeleyCursor::next(const Key& key)
		{
		Dbt data;

		ensureOpen();

		try
			{
			int result = cursor->get(&BerkeleyDatabase::ToDbt(key), &data, DB_SET);

			if(result == 0)
				return true;
			else if(result == DB_NOTFOUND)
				return false;
			else
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	unsigned long BerkeleyCursor::getCount(TransactionContext& transactionContext)
		{
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
		else if(totalCount == -1)
			{
			Dbc* countCursor = NULL;
			int result;

			try
				{
				if((result = cursor->dup(&countCursor, DB_POSITION)) == 0)
					{
					startCursor(*this, countCursor);
					// TODO Can we improve on an O(n) iteration to get a decent count estimate?
					for(totalCount = 0; next(countCursor, transactionContext); ++totalCount)
						{ }
					}
				else 
					throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
				}
			catch(DbDeadlockException& e)
				{ 
				closeCursor(countCursor);
				throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); 
				}
			catch(DbException& e)
				{ 
				closeCursor(countCursor);
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno());
				}

			closeCursor(countCursor);
			}
		
		return totalCount;
		}

	void BerkeleyCursor::remove()
		{
		int result;

		ensureOpen();

		try
			{
			if((result = cursor->del(0)) != 0)
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ 
			if(e.get_errno() == EACCES)
				throw ImplementationException("NOT_ALLOWED_ERR", ImplementationException::NOT_ALLOWED_ERR, e.get_errno());
			else
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno());
			}
		}

	void BerkeleyCursor::close()
		{
		lock_guard<mutex> guard(synchronization);
		int error = 0;

		if(isOpen)
			try 
				{ 
				isOpen = false;
				cursor->close(); 
				}
			catch(DbDeadlockException& e)
				{ error = e.get_errno(); }
			catch(DbException& e)
				{ error = e.get_errno(); }

		try
			{
			if(implicitTransaction != NULL)
				{
				implicitTransaction->commit(0);
				implicitTransaction = NULL;
				}
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }

		if(error != 0)
			throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, error);
		}

	void BerkeleyCursor::startCursor()
		{ startCursor(*this); }

	void BerkeleyCursor::startCursor(BerkeleyCursor& berkeleyCursor)
		{ startCursor(berkeleyCursor, berkeleyCursor.cursor); }

	void BerkeleyCursor::startCursor(BerkeleyCursor& berkeleyCursor, Dbc* cursor)
		{
		if(berkeleyCursor.isReversed && berkeleyCursor.openRight)
			startOpenRightIntervalReverseCursor(cursor, berkeleyCursor.right);
		else if(berkeleyCursor.isReversed)
			startClosedRightReverseCursor(cursor, berkeleyCursor.right);
		else if(berkeleyCursor.openLeft)
			startOpenLeftIntervalCursor(cursor, berkeleyCursor.left);
		else
			startClosedLeftCursor(cursor, berkeleyCursor.left);
		}

	void BerkeleyCursor::startClosedLeftCursor(Dbc* cursor, const Key& left)
		{
		int result;

		if(left.getType() == Data::Undefined)
			result = cursor->get(&Dbt(), &Dbt(), DB_FIRST);
		else 
			result = cursor->get(&BerkeleyDatabase::ToDbt(left), &Dbt(), DB_SET_RANGE);
		
		if(result == DB_NOTFOUND)
			throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR);
		else if(result != 0)
			throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
		}

	void BerkeleyCursor::startClosedRightReverseCursor(Dbc* cursor, const Key& right)
		{
		int result;
		Dbt key, data;

		if(right.getType() == Data::Undefined)
			result = cursor->get(&key, &data, DB_LAST);
		else if((result = cursor->get(&BerkeleyDatabase::ToDbt(right), &data, DB_SET_RANGE)) == 0)
			{
			// Iterate until we move off the key, and then go back one
			for((result = cursor->get(&key, &data, DB_CURRENT)); 
				result == 0 && BerkeleyDatabase::ToKey(key) == right;
				(result = cursor->get(&key, &data, DB_NEXT)));
			
			if(result == 0)
				cursor->get(&key, &data, DB_PREV);
			else
				result = 0;
			}
		
		if(result == DB_NOTFOUND)
			throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR, result);
		else if(result != 0)
			throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
		}

	void BerkeleyCursor::startOpenRightIntervalReverseCursor(Dbc* cursor, const Key& right)
		{
		int result;
		Dbt data;

		if(right.getType() == Data::Undefined)
			result = cursor->get(&Dbt(), &data, DB_LAST);
		else if((result = cursor->get(&BerkeleyDatabase::ToDbt(right), &data, DB_SET_RANGE)) == 0)
			result = cursor->get(&Dbt(), &data, DB_PREV);
		
		if(result == DB_NOTFOUND)
			throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR, result);
		else if(result != 0)
			throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
		}

	void BerkeleyCursor::startOpenLeftIntervalCursor(Dbc* cursor, const Key& left)
		{
		int result;
		Dbt* leftBound = &BerkeleyDatabase::ToDbt(left);
		Dbt data;

		if(left.getType() == Data::Undefined)
			result = cursor->get(&Dbt(), &Dbt(), DB_FIRST);
		else if((result = cursor->get(leftBound, &data, DB_SET)) == 0)
			{
			if((result = cursor->get(leftBound, &data, DB_NEXT)) == DB_NOTFOUND)
				throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR, result);
			else if(result != 0)
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
			}
		else 
			result = cursor->get(leftBound, &data, DB_SET_RANGE);
		
		if(result == DB_NOTFOUND)
			throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR, result);
		else if(result != 0)
			throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, result);
		}

	bool BerkeleyCursor::isOutOfRange(const Dbt& key)
		{ return isOutOfRange(BerkeleyDatabase::ToKey(key)); }

	bool BerkeleyCursor::isOutOfRange(const Key& key)
		{
		if(isReversed)
			return left.getType() != Data::Undefined &&
					(openLeft
						? left >= key
						: left > key);
		else
			return right.getType() != Data::Undefined &&
					(openRight
						? right <= key
						: right < key);
		}

	Dbc* BerkeleyCursor::makeCursor(Db& source, TransactionContext transactionContext)
		{
		DbTxn* transaction;
		Dbc* cursor;

		transaction = BerkeleyTransaction::ToDbTxn(transactionContext);

		if(transaction == NULL)
			{
			source.get_env()->txn_begin(NULL, &transaction, 0); 
			this->implicitTransaction = transaction;
			}
		else
			this->implicitTransaction = NULL;

		source.cursor(transaction, &cursor, 0);
		return cursor;
		}

	void BerkeleyCursor::ensureOpen()
		{
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
		}

	void BerkeleyCursor::closeCursor(Dbc* cursor)
		{
		try 
			{ if(cursor != NULL) cursor->close(); }
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}
	}
}
}
}