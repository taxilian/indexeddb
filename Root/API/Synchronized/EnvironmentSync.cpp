/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include <BrowserHost.h>
#include "EnvironmentSync.h"

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

EnvironmentSync::EnvironmentSync(const FB::BrowserHostPtr& host)
	: indexedDB(boost::make_shared<IndexedDatabase>(host))
	{ }

IndexedDatabasePtr EnvironmentSync::getIndexedDB()
	{ return indexedDB; }

}
}
}