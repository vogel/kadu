project (auto_hide)

find_package (Kadu REQUIRED)

set (SOURCES
	auto_hide.cpp
)

set (MOC_SOURCES
	auto_hide.h
)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui
)

kadu_plugin (auto_hide
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
)
