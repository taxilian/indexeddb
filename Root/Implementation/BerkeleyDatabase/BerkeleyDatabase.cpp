/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "BerkeleyObjectStore.h"
#include "BerkeleyDatabase.h"
#include "BerkeleyTransaction.h"
#include "BerkeleyDeadlockDetection.h"
#include "../ImplementationException.h"
#include "../Key.h"
#include "../Data.h"
#include "../../Support/DatabaseLocation.h"

using std::string;

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 
namespace BerkeleyDB
	{
	const string BerkeleyDatabase::metadataDatabaseSuffix = "__metadata";

	BerkeleyDatabase::BerkeleyDatabase(const string& origin, const string& name, const string& description, const bool modifyDatabase)
		: environment(0), name(name), origin(origin),
		  deadlockDetection(new BerkeleyDeadlockDetection(environment, 3000))
		{
		environment.set_lg_max(262144);
		environment.set_flags(DB_AUTO_COMMIT, 1);
		environment.set_timeout(2500, DB_SET_LOCK_TIMEOUT);
		environment.set_timeout(2500, DB_SET_TXN_TIMEOUT);
		environment.set_lk_detect(DB_LOCK_DEFAULT);
		environment.set_errcall(this->errorHandler);
		int environmentFlags = DB_CREATE | DB_INIT_LOCK | DB_INIT_MPOOL | DB_INIT_TXN | DB_INIT_LOG;

		try 
			{ 
			environment.open(DatabaseLocation::getEnvironmentPath(origin, name).c_str(), environmentFlags, 0); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }

		deadlockDetection->start();

		metadata.reset(new BerkeleyObjectStore(*this, name + metadataDatabaseSuffix, 
			ObjectStore::READ_WRITE, true, TransactionContext()));
		}

	BerkeleyDatabase::~BerkeleyDatabase()
		{ 
		deadlockDetection->stop();

		try
			{ metadata->close(); }
		catch(ImplementationException&) { }

		try
			{ environment.close(0); }
		catch(DbDeadlockException&) { }
		catch(DbException&) { }
		}

	void BerkeleyDatabase::removeObjectStore(const string& objectStoreName, TransactionContext& transactionContext)
		{
		DbTxn* transaction = BerkeleyTransaction::ToDbTxn(transactionContext);

		//TODO: Should be be using sub databases?  Is there any perf difference?
		try
			{ getEnvironment().dbremove(transaction, DatabaseLocation::getObjectStorePath(origin, this->name, objectStoreName).c_str(), NULL, 
				transaction == NULL ? DB_AUTO_COMMIT : 0); }
		catch(DbDeadlockException& e)
			{ throw ImplementationException("DEADLOCK_ERR", ImplementationException::DEADLOCK_ERR, e.get_errno()); }
		catch(DbException& e)
			{ throw ImplementationException("UNKNOWN_ERR", ImplementationException::UNKNOWN_ERR, e.get_errno()); }
		}

	void BerkeleyDatabase::errorHandler(const DbEnv *environment, const char *errpfx, const char *message)		
		{ }

	Dbt BerkeleyDatabase::ToDbt(const Data& data)
		{ return Dbt(const_cast<void*>(static_cast<const void*>(data)), data.getSize()); }

	Data BerkeleyDatabase::ToData(const Dbt& dbt)
		{ return Data(static_cast<unsigned char*>(dbt.get_data()) + 1, dbt.get_size() - 1, *static_cast<Data::ECMAType*>(dbt.get_data())); }

	Key BerkeleyDatabase::ToKey(const Dbt& key)
		{ return Key(static_cast<unsigned char*>(key.get_data()) + 1, key.get_size() - 1, *static_cast<Data::ECMAType*>(key.get_data())); }
	}
}
}
}