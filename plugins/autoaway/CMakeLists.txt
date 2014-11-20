project (autoaway)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

include (FindPkgConfig)

set (SOURCES
	autoaway.cpp
	autoaway-status-changer.cpp
)

set (MOC_SOURCES
	autoaway.h
	autoaway-status-changer.h
)

set (CONFIGURATION_FILES
	configuration/autoaway.ui
)

pkg_check_modules (INJEQT REQUIRED injeqt)

include_directories (${INJEQT_INCLUDEDIR})
link_directories (${INJEQT_LIBRARY_DIRS})
set (${INJEQT_LIBRARIES})

kadu_plugin (autoaway
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
	PLUGIN_LIBRARIES ${LIBRARIES}
)
