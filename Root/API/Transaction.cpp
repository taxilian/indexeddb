/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#include "Transaction.h"
#include "Database.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

Transaction::Transaction(Database& database, const bool isStatic)
	: database(database), isStatic(isStatic)
	{
	registerProperty("static", make_property(this, &Transaction::getStatic));
	registerProperty("db", make_property(this, &Transaction::getDatabaseObject));
	}

FB::JSOutObject Transaction::getDatabaseObject() 
	{ return &database; }

long Transaction::commit()
	{ getDatabase().onTransactionCommitted(*this); return 0; }

long Transaction::abort()
	{ getDatabase().onTransactionAborted(*this); return 0; }

}
}
}