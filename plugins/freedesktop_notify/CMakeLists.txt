project (freedesktop_notify)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (QT_USE_QTDBUS 1)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/gui/freedesktop-notify-configuration-ui-handler.cpp
	freedesktop-notify.cpp
	freedesktop-notify-plugin.cpp
)

set (MOC_SOURCES
	configuration/gui/freedesktop-notify-configuration-ui-handler.h
	freedesktop-notify.h
	freedesktop-notify-plugin.h
)

set (TRANSLATION_SOURCES
	translations/freedesktop_notify_cs.ts
	translations/freedesktop_notify_de.ts
	translations/freedesktop_notify_en.ts
	translations/freedesktop_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/freedesktop_notify.ui
)

kadu_plugin (freedesktop_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
