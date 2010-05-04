#include <string>
#include <sstream>
#include "BrowserHostWrapper.h"

#include "Root/API/Synchronized/EnvironmentSync.h"

class IndexedDatabasePluginAPI : public BrandonHaynes::IndexedDB::API::EnvironmentSync
{
public:
    IndexedDatabasePluginAPI(FB::BrowserHost host);

	FB::JSOutObject getKeyRange();
	FB::JSOutObject getCursor();

private:
	FB::BrowserHost host;
};
