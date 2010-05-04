#/**********************************************************\ 
# Auto-generated Windows project definition file for the
# IndexedDatabasePlugin project
#\**********************************************************/

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Win/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/*.cpp
    Win/*.h
    Win/*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    /D "_ATL_STATIC_REGISTRY"
)

SOURCE_GROUP(Win FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

set (BDB_INCLUDE_DIR 
		"$ENV{PROGRAMFILES}/Oracle/Berkeley DB 4.8.26/include"
		"$ENV{PROGRAMFILES(x86)}/Oracle/Berkeley DB 4.8.26/include")
set (BDB_LIBRARY_DIR 
		"$ENV{PROGRAMFILES}/Oracle/Berkeley DB 4.8.26/lib/libdb48d.lib"
		"$ENV{PROGRAMFILES(x86)}/Oracle/Berkeley DB 4.8.26/lib/libdb48d.lib"
		)

set (BOOST_INCLUDE_DIR 
	"$ENV{BOOST_HOME}/include/boost-1_40")
set (BOOST_LIBRARY_DIR 
	"$ENV{BOOST_HOME}/lib/libboost_thread-vc90-mt-sgd-1_40.lib"
	"$ENV{BOOST_HOME}/lib/libboost_date_time-vc90-mt-sgd-1_40.lib"
	"$ENV{BOOST_HOME}/lib/libboost_filesystem-vc90-mt-sgd-1_40.lib"
	"$ENV{BOOST_HOME}/lib/libboost_system-vc90-mt-sgd-1_40.lib"
	)

include_directories(${BDB_INCLUDE_DIR}) 
include_directories(${BOOST_INCLUDE_DIR}) 

add_library(${PROJNAME} SHARED ${SOURCES})

set_target_properties (${PROJNAME} PROPERTIES
    OUTPUT_NAME np${PLUGIN_NAME}
    PROJECT_LABEL ${PROJNAME}
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
    )

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    ${BDB_LIBRARY_DIR}
    ${BOOST_LIBRARY_DIR}
    )

add_dependencies(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    )

set(WIX_HEAT_FLAGS
    -gg                 # Generate GUIDs
    -srd                # Suppress Root Dir
    -cg PluginDLLGroup  # Set the Component group name
    -dr INSTALLDIR      # Set the directory ID to put the files in
    )

add_wix_installer( ${PLUGIN_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/IndexedDatabasePluginInstaller.wxs
    PluginDLLGroup
    ${BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
    ${BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/np${PLUGIN_NAME}.dll
    ${PROJNAME}
    )

