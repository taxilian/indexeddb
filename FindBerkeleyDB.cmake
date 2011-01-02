
if (NOT BERKELEYDB_DIR)
    if (WIN32)
        set(_START_SEARCH_DIRS
            $ENV{PROGRAMFILES}/Oracle
            )
        foreach(SEARCH_DIR ${_START_SEARCH_DIRS})
            if (EXISTS "${SEARCH_DIR}")
                file (GLOB BDB_DIRS
                    ${SEARCH_DIR}/Berkeley*DB*)
                foreach(CUR_DIR ${BDB_DIRS})
                    set(POSSIBLE_DIRS ${POSSIBLE_DIRS} "${BDB_DIRS}/include")
                endforeach()
            endif()
        endforeach()
        find_file(BDB_INCLUDE_FILE
            db.h
        PATHS
            ${POSSIBLE_DIRS}
        )
        GET_FILENAME_COMPONENT(_BERKELEYDB_INCLUDE_DIR ${BDB_INCLUDE_FILE} PATH CACHE)
        GET_FILENAME_COMPONENT(_BERKELEYDB_DIR "${_BERKELEYDB_INCLUDE_DIR}/.." ABSOLUTE CACHE)
        set (BERKELEYDB_INCLUDE_DIR ${_BERKELEYDB_INCLUDE_DIR} CACHE INTERNAL "BDB includes location")
        set (BERKELEYDB_LIB_DIR ${_BERKELEYDB_DIR}/lib CACHE INTERNAL "BDB lib location")
        file(GLOB _BERKELEYDB_LIBRARIES
            ${BERKELEYDB_LIB_DIR}/libdb*.lib
            )
        set (BERKELEYDB_LIBRARIES ${_BERKELEYDB_LIBRARIES} CACHE INTERNAL "BDB libraries")
        message("-- Found BDB include dir: ${BERKELEYDB_INCLUDE_DIR}")
        message("-- Found BDB libs: ${BERKELEYDB_LIBRARIES}")
    endif()
endif()

set(BERKELEYDB_INCLUDE_DIR ${BERKELEYDB_INCLUDE_DIR} PARENT_SCOPE)
set(BERKELEYDB_LIBRARIES ${BERKELEYDB_LIBRARIES} PARENT_SCOPE)
