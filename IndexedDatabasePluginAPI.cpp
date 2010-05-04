#include "IndexedDatabasePluginAPI.h"
#include "Source/API/KeyRange.h"
#include "Source/API/Cursor.h"

IndexedDatabasePluginAPI::IndexedDatabasePluginAPI(FB::BrowserHost host) 
	: host(host), EnvironmentSync(host)
	{
    registerProperty("indexedDB", make_property(this, &IndexedDatabasePluginAPI::getIndexedDB));
    registerProperty("IDBKeyRange", make_property(this, &IndexedDatabasePluginAPI::getKeyRange));
    registerProperty("IDBCursor", make_property(this,&IndexedDatabasePluginAPI::getCursor));
	}

FB::JSOutObject IndexedDatabasePluginAPI::getKeyRange()
	{ return new BrandonHaynes::IndexedDB::API::KeyRange(); }

FB::JSOutObject IndexedDatabasePluginAPI::getCursor()
	{ return static_cast<FB::JSOutObject>(BrandonHaynes::IndexedDB::API::Cursor::getInstance()); }
