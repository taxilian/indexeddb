#include "BerkeleyIndex.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyObjectStore.h"
#include "BerkeleyTransaction.h"
#include "../Key.h"
#include "../KeyGenerator.h"
#include "../ImplementationException.h"
#include "../../Support/DatabaseLocation.h"

using boost::mutex;
using boost::lock_guard;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	BerkeleyIndex::BerkeleyIndex(BerkeleyObjectStore& objectStore, const std::string& name, const std::auto_ptr<IndexedDB::Implementation::KeyGenerator>& keyGenerator, const bool unique, TransactionContext& transactionContext, const bool create)
		: implementation(objectStore.getImplementation().get_env(), 0), isOpen(true)
		{
		DatabaseLocation::ensurePathValid(name);

		if(!unique)
			implementation.set_flags(DB_DUPSORT);

		//TODO: What happens if an index has the same name as a database?
		try 
			{ 
			implementation.open(BerkeleyTransaction::ToDbTxn(transactionContext), name.c_str(), NULL, DB_BTREE, 
				DB_AUTO_COMMIT | (create ? DB_CREATE : 0), 0); 

			implementation.set_app_private(static_cast<void*>(keyGenerator.get()));

			objectStore.getImplementation().associate(BerkeleyTransaction::ToDbTxn(transactionContext), &implementation, 
				callback, DB_CREATE); 
			} 
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	BerkeleyIndex::~BerkeleyIndex(void)
		{ 
		try
			{ close(); }
		catch(ImplementationException&)
			{ }
		}

	Key BerkeleyIndex::getPrimaryKey(const Key& secondaryKey, TransactionContext& transactionContext)
		{
		Dbt data, primaryKey;

		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if(implementation.pget(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), &primaryKey, &data, 0) == 0)
				return BerkeleyDatabase::ToKey(primaryKey);
			else
				return Key::getUndefinedKey();
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	Data BerkeleyIndex::get(const Key& secondaryKey, TransactionContext& transactionContext)
		{
		Dbt data;

		try
			{
			if(!isOpen)
				throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);
			else if(implementation.get(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), &data, 0) == 0)
				return BerkeleyDatabase::ToData(data);
			else
				return Data::getUndefinedData();
			}
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyIndex::put(const Key& secondaryKey, const Data& primaryKey, const bool noOverwrite, TransactionContext& transactionContext)
		{ throw ImplementationException(ImplementationException::NOT_ALLOWED_ERR); }

	void BerkeleyIndex::remove(const Key& secondaryKey, TransactionContext& transactionContext)
		{ 
		if(!isOpen)
			throw ImplementationException("NON_TRANSIENT_ERR", ImplementationException::NON_TRANSIENT_ERR);

		try 
			{ implementation.del(BerkeleyTransaction::ToDbTxn(transactionContext), &BerkeleyDatabase::ToDbt(secondaryKey), 0); }
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyIndex::close()
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
			catch(DbException& e)
				{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	int BerkeleyIndex::callback(Db *secondary, const Dbt *key, const Dbt *data, Dbt *result)
		{
		try
			{
			const KeyGenerator* keyGenerator = static_cast<const KeyGenerator*>(secondary->get_app_private());

			Data& value = BerkeleyDatabase::ToData(*data);
			Key indexKey(keyGenerator->generateKey(value));

			void* keyData = malloc(indexKey.getSize());
			memcpy(keyData, indexKey, indexKey.getSize());

			memset(result, 0, sizeof(Dbt));
			//result->set_flags(DB_DBT_APPMALLOC);  //TODO Why does this fail?
			result->set_data(keyData);
			result->set_size(indexKey.getSize());

			return 0;
			}
		catch(std::bad_cast&)
			{ throw ImplementationException("Key generator cast failed.", ImplementationException::UNKNOWN_ERR); }
		}
	}
}
}
}