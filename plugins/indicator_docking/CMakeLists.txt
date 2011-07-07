project (indicator_docking)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)
include (FindPkgConfig)

find_package(PkgConfig REQUIRED) 
pkg_search_module(INDICATEQT REQUIRED indicate-qt) 

include_directories(${INDICATEQT_INCLUDE_DIRS}) 

set (SOURCES
	indicator_docking.cpp
	indicator_docking-plugin.cpp
)

set (MOC_SOURCES
	indicator_docking.h
	indicator_docking-plugin.h
)

kadu_plugin (indicator_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${INDICATEQT_LIBRARIES} 
)

if (NOT INDICATORS_PARENT_DIR)
    set (INDICATORS_PARENT_DIR "/usr/share")  
endif (NOT INDICATORS_PARENT_DIR)

# Pin Kadu to Messangigng Menu
file (WRITE ${CMAKE_BINARY_DIR}/kadu "${KADU_DESKTOP_FILE_DIR}/${KADU_DESKTOP_FILE_NAME}\n") 
install (FILES ${CMAKE_BINARY_DIR}/kadu DESTINATION ${INDICATORS_PARENT_DIR}/indicators/messages/applications/)
