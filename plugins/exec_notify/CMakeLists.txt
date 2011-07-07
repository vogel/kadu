project (exec_notify)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	exec_notify.cpp
	exec-notify-plugin.cpp
)

set (MOC_SOURCES
	exec_notify.h
	exec-notify-plugin.h
)

set (TRANSLATION_SOURCES
	translations/exec_notify_en.ts
	translations/exec_notify_pl.ts
)

kadu_plugin (exec_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
