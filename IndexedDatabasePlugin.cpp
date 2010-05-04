/**********************************************************\ 

  Auto-generated IndexedDatabasePlugin.cpp

  This file contains the auto-generated main plugin object 
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include "NpapiTypes.h"
#include "IndexedDatabasePluginAPI.h"

#include "IndexedDatabasePlugin.h"

void IndexedDatabasePlugin::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void IndexedDatabasePlugin::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


IndexedDatabasePlugin::IndexedDatabasePlugin()
{
}

IndexedDatabasePlugin::~IndexedDatabasePlugin()
{
}

FB::JSAPI* IndexedDatabasePlugin::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new IndexedDatabasePluginAPI(m_host);
}

bool IndexedDatabasePlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool IndexedDatabasePlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool IndexedDatabasePlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
