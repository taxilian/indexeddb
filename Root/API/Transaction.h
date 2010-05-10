/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_TRANSACTION_H
#define BRANDONHAYNES_INDEXEDDB_API_TRANSACTION_H

#include <JSAPIAuto.h>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class Database;

///<summary>
/// This class represents a transaction in the Indexed Database API.
///</summary>
class Transaction : public FB::JSAPIAuto
{
public:
	// Gets a flag indicating whether this transaction is static (true) or dynamic (false).  See the spec for definitions.
	bool getStatic() { return isStatic; }
	Database& getDatabase() { return database; }

	// Not much action here.  Transactions commit or abort...
	virtual void commit();
	virtual void abort();

protected:
	Transaction(Database& database, const bool isStatic);

private:
	Database& database;
	const bool isStatic;
	
	// Helper method to translate the database accessor into a FireBreath compatible form
	FB::JSOutObject getDatabaseObject();
};

}
}
}

#endif