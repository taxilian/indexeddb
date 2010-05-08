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

class Transaction : public FB::JSAPIAuto
{
public:
	bool getStatic() { return isStatic; }
	Database& getDatabase() { return database; }

	//TODO change to void return type after 1.0 upgrade
	virtual long commit();
	virtual long abort();

protected:
	Transaction(Database& database, const bool isStatic);

private:
	Database& database;
	const bool isStatic;
	
	FB::JSOutObject getDatabaseObject();
};

}
}
}

#endif