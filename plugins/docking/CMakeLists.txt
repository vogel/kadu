project (docking)

find_package (Kadu REQUIRED)

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
endif ()

set (CONFIGURATION_FILES
	data/configuration/docking.ui
)

kadu_plugin (docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_BUILDDEF DOCKING_BUILD
)
