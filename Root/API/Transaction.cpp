/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "Transaction.h"
#include "Database.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

Transaction::Transaction(const DatabasePtr& database, const bool isStatic)
	: database(database), isStatic(isStatic)
	{
	registerProperty("static", make_property(this, &Transaction::getStatic));
	registerProperty("db", make_property(this, &Transaction::getDatabaseObject));
	}

DatabasePtr Transaction::getDatabaseObject() 
	{ return database; }

void Transaction::commit()
	{ getDatabase()->onTransactionCommitted(FB::ptr_cast<Transaction>(shared_from_this())); }

void Transaction::abort()
	{ getDatabase()->onTransactionAborted(FB::ptr_cast<Transaction>(shared_from_this())); }

}
}
}