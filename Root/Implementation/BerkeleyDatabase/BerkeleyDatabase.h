/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYDATABASE_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_BERKELEYDB_BERKELEYDATABASE_H

#include <string>
#include <db_cxx.h>
#include "../Database.h"
#include "../Transaction.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	class ObjectStore; 
	class Key;
	class Data;

	namespace BerkeleyDB {

		class BerkeleyDeadlockDetection;

		class BerkeleyDatabase : public Database
			{
			public:
				BerkeleyDatabase(const std::string& origin, const std::string& name, const std::string& description, const bool modifyDatabase);
				virtual ~BerkeleyDatabase(void);

				virtual void removeObjectStore(const std::string& objectStoreName, TransactionContext& transactionContext);
				virtual ObjectStore& getMetadata() { return *metadata; }

				static Dbt ToDbt(const Data& data);
				static Data ToData(const Dbt& dbt);
				static Key ToKey(const Dbt& key);

				DbEnv& getEnvironment() { return environment; }

			private:
				DbEnv environment;
				std::auto_ptr<BerkeleyDeadlockDetection> deadlockDetection;
				std::auto_ptr<ObjectStore> metadata;
				const std::string origin;
				const std::string name;

				static const std::string metadataDatabaseSuffix;

				static void errorHandler(const DbEnv *environment, const char *errpfx, const char *message);
			};
		}
	}
}
}

#endif