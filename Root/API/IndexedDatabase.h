/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_API_INDEXEDDATABASE_H
#define BRANDONHAYNES_INDEXEDDB_API_INDEXEDDATABASE_H

#include <JSAPIAuto.h>
#include <APITypes.h>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

FB_FORWARD_PTR(Database);

///<summary>
/// This class represents the IndexedDB interface in the Indexed Database API.  Because our 
/// namespace is also called "IndexedDB", we use an alternate name here.
///</summary>
class IndexedDatabase : public FB::JSAPIAuto
{
public:
	IndexedDatabase(FB::BrowserHostPtr host);

	DatabasePtr open(const std::string& name, const std::string& description, const FB::CatchAll& args);

protected:
	FB::BrowserHostPtr host;
};

}
}
}

#endif