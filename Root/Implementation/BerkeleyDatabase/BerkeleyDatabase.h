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

		///<summary>
		/// This class represents a Indexed Database API database implementation (which is represented, confusingly,
		/// by a Berkeley DB environment).
		///</summary>
		class BerkeleyDatabase : public Database
			{
			public:
				BerkeleyDatabase(const std::string& origin, const std::string& name, const std::string& description, const bool modifyDatabase);
				virtual ~BerkeleyDatabase(void);

				virtual void removeObjectStore(const std::string& objectStoreName, TransactionContext& transactionContext);
				virtual ObjectStore& getMetadata() { return *metadata; }

				// Utility methods to convert between the implementation-exposing Data/Key objects and underlying
				// BerkeleyDB Dbts.  Used by most of the other Berkeley DB implementation classes. 
				static Dbt ToDbt(const Data& data);
				static Data ToData(const Dbt& dbt);
				static Key ToKey(const Dbt& key);

				// Not a fan of exposing the environment in this way, but otherwise we'd need several friends.
				DbEnv& getEnvironment() { return environment; }

			private:
				DbEnv environment;

				// Managed thread associated with this environment to detect lock and transaction timeouts
				std::auto_ptr<BerkeleyDeadlockDetection> deadlockDetection;

				// An object store containing metdata for this environment
				std::auto_ptr<ObjectStore> metadata;

				const std::string origin;
				const std::string name;

				// An fixed suffix for metadatabase naming (e.g. "__metadata")
				static const std::string metadataDatabaseSuffix;

				// Empty implementation; set a breakpoint here for debugging.
				static void errorHandler(const DbEnv *environment, const char *errpfx, const char *message);
			};
		}
	}
}
}

#endif