/**********************************************************\ 

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods 
  for the ${PLUGIN.name} project

\**********************************************************/

#include "FactoryDefinitions.h"
#include "IndexedDatabasePlugin.h"

FB::PluginCore *_getMainPlugin()
{
    return new IndexedDatabasePlugin();
}

void GlobalPluginInitialize()
{
    IndexedDatabasePlugin::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    IndexedDatabasePlugin::StaticDeinitialize();
}
