project (auto_hide)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

include (FindPkgConfig)

set (SOURCES
	auto_hide.cpp
)

set (MOC_SOURCES
	auto_hide.h
)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui
)

pkg_check_modules (INJEQT REQUIRED injeqt)

include_directories (${INJEQT_INCLUDEDIR})
link_directories (${INJEQT_LIBRARY_DIRS})
set (${INJEQT_LIBRARIES})

kadu_plugin (auto_hide
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
	PLUGIN_LIBRARIES ${LIBRARIES}
)
