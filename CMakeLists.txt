#/**********************************************************\ 
# Auto-generated CMakeLists.txt for the IndexedDatabasePlugin project
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME ${PLUGIN_NAME})

browserplugin_project(${PROJNAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    )

file (GLOB SYNC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/API/Synchronized/*.cpp
    root/API/Synchronized/*.h
)
source_group(Synchronized FILES ${SYNC_FILES})

file (GLOB API_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/API/*.cpp
    root/API/*.h
)
source_group(API FILES ${API_FILES})

file (GLOB BERK_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/Implementation/BerkeleyDatabase/*.cpp
    root/Implementation/BerkeleyDatabase/*.h
)
source_group(BerklyDatabase FILES ${BERK_FILES})

file (GLOB IMPL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/Implementation/*.cpp
    root/Implementation/*.h
)
source_group(Implementation FILES ${IMPL_FILES})

file (GLOB SUPPORT_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/Support/*.cpp
    root/Support/*.h
)
source_group(Support FILES ${SUPPORT_FILES})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${SYNC_FILES}
    ${API_FILES}
    ${BERK_FILES}
    ${SUPPORT_FILES}
    ${GENERATED}
    )

# This will include Win/projectDef.cmake, Linux/projectDef.cmake, etc
include_platform()

