project (docking)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

kadu_api_directories (plugins/docking
	.
)

set (SOURCES
	docking-plugin.cpp
	docking.cpp
)

set (MOC_SOURCES
	docking-plugin.h
	docking.h
)

if (APPLE)
	list (APPEND SOURCES mac_docking_helper.cpp)
	list (APPEND MOC_SOURCES mac_docking_helper.h)
	SET_SOURCE_FILES_PROPERTIES(mac_docking_helper.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif (APPLE)

set (TRANSLATION_SOURCES
	translations/docking_de.ts
	translations/docking_en.ts
	translations/docking_it.ts
	translations/docking_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/docking.ui
)

kadu_plugin (docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_BUILDDEF DOCKING_BUILD
)
