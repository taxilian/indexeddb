#ifndef BRANDONHAYNES_INDEXEDDB_API_INDEXEDDATABASE_H
#define BRANDONHAYNES_INDEXEDDB_API_INDEXEDDATABASE_H

#include <JSAPIAuto.h>
#include <APITypes.h>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace API { 

class IndexedDatabase : public FB::JSAPIAuto
{
public:
	IndexedDatabase(FB::BrowserHost host);

	FB::JSOutObject open(const std::string& name, const std::string& description, const FB::CatchAll& args);

protected:
	FB::BrowserHost host;
};

}
}
}

#endif