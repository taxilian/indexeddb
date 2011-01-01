#include <string>
#include <sstream>
#include "BrowserHostWrapper.h"

#include "Root/API/Synchronized/EnvironmentSync.h"

#ifndef BRANDONHAYNES_INDEXEDDB_INDEXEDDATABASEPLUGINAPI_H
#define BRANDONHAYNES_INDEXEDDB_INDEXEDDATABASEPLUGINAPI_H

///<summary>
/// Primary entry point into our project.  We expose a few other stub instances so that
/// user agents may retreive constants.
///</summary>
class IndexedDatabasePluginAPI : public BrandonHaynes::IndexedDB::API::EnvironmentSync
{
public:
    IndexedDatabasePluginAPI(FB::BrowserHostPtr host);

	// Expose a stub keyrange for access to constants
	FB::JSAPIPtr getKeyRange();
	// Expose a stub cursor for access to constants
	FB::JSAPIPtr getCursor();

private:
	FB::BrowserHostPtr host;
};

#endif