/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_ENVIRONMENTSYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_ENVIRONMENTSYNC_H

#include <JSAPIAuto.h>
#include "../IndexedDatabase.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

///<summary>
/// This class represents a database environment.  It is both the entry-point into the API, and also
/// the interface-point between FireBreath code and our project.
///</summary>
class EnvironmentSync : public FB::JSAPIAuto
{
public:
	EnvironmentSync(FB::BrowserHost host);

	// Per spec, returns a reference to the IndexedDB API handle
	FB::JSOutObject getIndexedDB();

private:
	FB::AutoPtr<IndexedDatabase> indexedDB;
};

}
}
}

#endif