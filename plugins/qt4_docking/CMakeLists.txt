project (qt4_docking)

find_package (Kadu REQUIRED)

kadu_api_directories (plugins/qt4_docking
	.
)

set (SOURCES
	qt4_docking.cpp
	qt4-docking-plugin.cpp
)

set (MOC_SOURCES
	qt4_docking.h
	qt4-docking-plugin.h
)

kadu_plugin (qt4_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
	PLUGIN_BUILDDEF QT4DOCKAPI_BUILD
)
