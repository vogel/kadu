project (qt4_docking)

cmake_minimum_required (VERSION 2.8)


find_package (Kadu REQUIRED CONFIG)

find_package (KF5Notifications)
#target_link_libraries (qt4_docking KF5::Notifications)

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
	PLUGIN_LIBRARIES "KF5::Notifications"
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
