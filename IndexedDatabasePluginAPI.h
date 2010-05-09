#include <string>
#include <sstream>
#include "BrowserHostWrapper.h"

#include "Root/API/Synchronized/EnvironmentSync.h"

///<summary>
/// Primary entry point into our project.  We expose a few other stub instances so that
/// user agents may retreive constants.
///</summary>
class IndexedDatabasePluginAPI : public BrandonHaynes::IndexedDB::API::EnvironmentSync
{
public:
    IndexedDatabasePluginAPI(FB::BrowserHost host);

	// Expose a stub keyrange for access to constants
	FB::JSOutObject getKeyRange();
	// Expose a stub cursor for access to constants
	FB::JSOutObject getCursor();

private:
	FB::BrowserHost host;
};
