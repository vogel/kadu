project (hints)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	hint.cpp
	hint_manager.cpp
	hint-over-user-configuration-window.cpp
	hints_configuration_widget.cpp
	hints-configuration-window.cpp
	hints-configuration-ui-handler.cpp
	hints-plugin.cpp
)

set (MOC_SOURCES
	hint.h
	hint_manager.h
	hint-over-user-configuration-window.h
	hints_configuration_widget.h
	hints-configuration-window.h
	hints-configuration-ui-handler.h
	hints-plugin.h
)

set (CONFIGURATION_FILES
	configuration/hint-over-user.ui
	configuration/hints.ui
	configuration/hints-advanced.ui
	configuration/hints-notifier.ui
)

kadu_plugin (hints
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
