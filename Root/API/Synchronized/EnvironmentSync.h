/**********************************************************\
See http://code.google.com/p/indexeddb for license
Copyright Brandon Haynes
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_SYNC_ENVIRONMENTSYNC_H
#define BRANDONHAYNES_INDEXEDDB_API_SYNC_ENVIRONMENTSYNC_H

#include <JSAPIAuto.h>
#include "../IndexedDatabase.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class EnvironmentSync : public FB::JSAPIAuto
{
public:
	EnvironmentSync(FB::BrowserHost host);

	FB::JSOutObject getIndexedDB();

private:
	FB::AutoPtr<IndexedDatabase> indexedDB;
};

}
}
}

#endif