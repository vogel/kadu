project (indicator_docking)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

find_package(PkgConfig REQUIRED) 
pkg_check_modules(INDICATEQT REQUIRED indicate-qt) 

include_directories (${KADU_INCLUDE_DIR}/kadu-core)
include_directories(${INDICATEQT_INCLUDE_DIRS}) 

link_directories(${INDICATEQT_LIBRARY_DIRS}) 

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
)

# Pin Kadu to Messangigng Menu
FILE(WRITE "kadu" ${KADU_DESKTOP_FILE_DIR}/${KADU_DESKTOP_FILE_NAME})
INSTALL(FILES kadu DESTINATION /usr/share/indicators/messages/applications/)

target_link_libraries(indicator_docking ${INDICATEQT_LIBRARIES})
