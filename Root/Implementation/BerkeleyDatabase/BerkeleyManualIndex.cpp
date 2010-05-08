/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "../Key.h"
#include "../ImplementationException.h"
#include "BerkeleyManualIndex.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyObjectStore.h"
#include "BerkeleyTransaction.h"

using std::string;
using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyManualIndex::BerkeleyManualIndex(BerkeleyObjectStore& objectStore, const string& name, const bool unique, TransactionContext& transactionContext, const bool create)
		: implementation(objectStore.getImplementation().get_env(), 0), 
		  objectStore(objectStore)
		{
		if(!unique)
			implementation.set_flags(DB_DUPSORT);

		//TODO: What happens if an index has the same name as a database?
		try 
			{ implementation.open(BerkeleyTransaction::ToDbTxn(transactionContext), name.c_str(), NULL, DB_BTREE, DB_AUTO_COMMIT | (create ? DB_CREATE : 0), 0); } 
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }

		isOpen = true;
		}

	BerkeleyManualIndex::~BerkeleyManualIndex(void)
		{ 
		try
			{ close(); }
		catch(ImplementationException&)
			{ }
		}

	Key BerkeleyManualIndex::getPrimaryKey(const Key& secondaryKey, TransactionContext& transactionContext)
		{
		Dbt data, primaryKey;

		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if(implementation.get(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), &primaryKey, 0) == 0)
				return ensurePrimaryKeyExists(primaryKey, secondaryKey, transactionContext);
			else
				return Key::getUndefinedKey();
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	Data BerkeleyManualIndex::get(const Key& secondaryKey, TransactionContext& transactionContext)
		{
		Dbt primaryKey;

		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if(implementation.get(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), &primaryKey, 0) == 0)
				return ensurePrimaryKeyExists(objectStore.get(BerkeleyDatabase::ToKey(primaryKey), transactionContext), secondaryKey, transactionContext);
			else
				return Data::getUndefinedData();
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyManualIndex::put(const Key& secondaryKey, const Data& primaryKey, const bool noOverwrite, TransactionContext& transactionContext)
		{ 
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
		else if(objectStore.get(Key(primaryKey), transactionContext) == Data::getUndefinedData())
			throw ImplementationException("CONSTRAINT_ERR", ImplementationException::CONSTRAINT_ERR);

		try 
			{ 
			if(implementation.put(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), &BerkeleyDatabase::ToDbt(primaryKey), noOverwrite ? DB_NOOVERWRITE : 0) == DB_KEYEXIST)
				throw ImplementationException("CONSTRAINT_ERR", ImplementationException::CONSTRAINT_ERR, DB_KEYEXIST);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyManualIndex::remove(const Key& secondaryKey, TransactionContext& transactionContext)
		{ 
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);

		try 
			{ 
			if(implementation.del(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), 0) == DB_NOTFOUND)
				throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR);
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyManualIndex::close()
		{
		lock_guard<mutex> guard(synchronization);

		if(isOpen)
			try 
				{ 
				isOpen = false;
				implementation.close(0); 
				}
			catch(DbDeadlockException& e)
				{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
			catch(DbException &e) 
				{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	Key BerkeleyManualIndex::ensurePrimaryKeyExists(const Dbt& primaryKeyDbt, const Key& secondaryKey, TransactionContext& transactionContext)
		{
		Key& primaryKey = BerkeleyDatabase::ToKey(primaryKeyDbt);
		if(objectStore.get(primaryKey, transactionContext) == Data::getUndefinedData())
			{
			remove(secondaryKey, transactionContext);
			throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR);
			}
		return primaryKey;
		}

	Data BerkeleyManualIndex::ensurePrimaryKeyExists(const Data& data, const Key& secondaryKey, TransactionContext& transactionContext)
		{
		if(data == Data::getUndefinedData())
			{
			remove(secondaryKey, transactionContext);
			throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR);
			}
		return data;
		}
	}
}
}
}