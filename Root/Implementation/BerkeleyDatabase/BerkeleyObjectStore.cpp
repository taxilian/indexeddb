#include <atlstr.h>
#include "BerkeleyObjectStore.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyTransaction.h"
#include "..\ImplementationException.h"
#include "..\Key.h"
#include "..\Data.h"
#include "..\..\Support/DatabaseLocation.h"

using std::string;
using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyObjectStore::BerkeleyObjectStore(BerkeleyDatabase& database, const string& name, const bool autoIncrement, TransactionContext& transactionContext)
		: implementation(&database.getEnvironment(), 0), readOnly(false), isOpen(true)
		{
		DatabaseLocation::ensurePathValid(name);
		DbTxn* transaction = BerkeleyTransaction::ToDbTxn(transactionContext);

		try 
			{ getImplementation().open(transaction, name.c_str(), NULL, DB_BTREE, DB_EXCL | DB_CREATE | (transaction == NULL ? DB_AUTO_COMMIT : 0), 0); }
		catch(DbException &e) 
			{ 
			if(e.get_errno() == EPERM) // Cross-origin attempt!
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR, e.get_errno()); 
			else
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); 
			}
		}

	BerkeleyObjectStore::BerkeleyObjectStore(BerkeleyDatabase& database, const string& name, const Mode mode, const bool create, TransactionContext& transactionContext)
		: implementation(&database.getEnvironment(), 0), readOnly(mode != ObjectStore::READ_WRITE), isOpen(true)
		{
		DatabaseLocation::ensurePathValid(name);

		try 
			{ getImplementation().open(BerkeleyTransaction::ToDbTxn(transactionContext), name.c_str(), NULL, DB_BTREE, 
				(readOnly ? DB_RDONLY : 0) | (create ? DB_CREATE : 0) | DB_AUTO_COMMIT, 0); }
		catch(DbException &e)
			{ 
			if(e.get_errno() == ENOENT)
				throw ImplementationException("NOT_FOUND_ERR", ImplementationException::NOT_FOUND_ERR, e.get_errno());
			else
				throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno());
			}
		}

	BerkeleyObjectStore::~BerkeleyObjectStore()
		{ close(); }

	Data BerkeleyObjectStore::get(const Key& key, TransactionContext& transactionContext)
		{
		Dbt data;

		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if(getImplementation().get(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(key), &data, 0) == 0)
				return BerkeleyDatabase::ToData(data);
			else
				return Data::getUndefinedData();
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	bool BerkeleyObjectStore::exists(const Key& key, TransactionContext& transactionContext)
		{
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
		try
			{ return getImplementation().exists(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(key), 0) != DB_NOTFOUND; }
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyObjectStore::put(const Key& key, const Data& data, const bool noOverwrite, TransactionContext& transactionContext)
		{
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
		else if(readOnly)
			throw ImplementationException("NOT_ALLOWED_ERR", ImplementationException::NOT_ALLOWED_ERR);

		try 
			{ 
			getImplementation().put(BerkeleyTransaction::ToDbTxn(transactionContext),
				&BerkeleyDatabase::ToDbt(key), 
				&BerkeleyDatabase::ToDbt(data), 
				noOverwrite ? DB_NOOVERWRITE : 0); }
		catch(DbDeadlockException &e) 
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}
	
	void BerkeleyObjectStore::remove(const Key& key, TransactionContext& transactionContext)
		{ 
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
		else if(readOnly)
			throw ImplementationException("NOT_ALLOWED_ERR", ImplementationException::NOT_ALLOWED_ERR);

		try 
			{ getImplementation().del(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(key), 0); }
		catch(DbDeadlockException &e) 
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException &e) 
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyObjectStore::close()
		{
		lock_guard<mutex> guard(synchronization);

		if(isOpen)
			try 
				{ 
				isOpen = false;
				getImplementation().close(0); 
				}
			catch(DbException &e) 
				{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyObjectStore::removeIndex(const string& name, TransactionContext& transactionContext)
		{
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);

		const char* home;
		getImplementation().get_env()->get_home(&home);

		try
			{ 
			DbTxn* transaction = BerkeleyTransaction::ToDbTxn(transactionContext);
			getImplementation().get_env()->dbremove(BerkeleyTransaction::ToDbTxn(transactionContext), 
												(string(home) + "\\" + name).c_str(), NULL, 
												transaction == NULL ? DB_AUTO_COMMIT : 0); 
			}
		catch(DbDeadlockException &e) 
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}
	}
}
}
}