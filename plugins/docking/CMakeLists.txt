project (docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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
	set_source_files_properties (SOURCE mac_docking_helper.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -x objective-c++")
endif ()

set (CONFIGURATION_FILES
	data/configuration/docking.ui
)

kadu_plugin (docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
