/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <BrowserHostWrapper.h>
#include "EnvironmentSync.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

EnvironmentSync::EnvironmentSync(FB::BrowserHostPtr host)
	: indexedDB(new IndexedDatabase(host))
	{ }

FB::JSAPIPtr EnvironmentSync::getIndexedDB()
	{ return static_cast<FB::JSAPIPtr>(indexedDB); }

}
}
}