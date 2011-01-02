#include <boost/make_shared.hpp>
#include "IndexedDatabasePluginAPI.h"
#include "Root/API/KeyRange.h"
#include "Root/API/Cursor.h"

using BrandonHaynes::IndexedDB::API::KeyRange;
IndexedDatabasePluginAPI::IndexedDatabasePluginAPI(FB::BrowserHostPtr host) 
	: host(host), EnvironmentSync(host)
	{
    registerProperty("indexedDB", make_property(this, &IndexedDatabasePluginAPI::getIndexedDB));
    registerProperty("IDBKeyRange", make_property(this, &IndexedDatabasePluginAPI::getKeyRange));
    registerProperty("IDBCursor", make_property(this,&IndexedDatabasePluginAPI::getCursor));
	}

FB::JSAPIPtr IndexedDatabasePluginAPI::getKeyRange()
	{ return boost::shared_ptr<KeyRange>(new KeyRange()); }

FB::JSAPIPtr IndexedDatabasePluginAPI::getCursor()
	{ return BrandonHaynes::IndexedDB::API::Cursor::getInstance(); }
