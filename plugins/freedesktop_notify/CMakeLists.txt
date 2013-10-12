project (freedesktop_notify)

cmake_minimum_required (VERSION 2.8)

set (QT_USE_QTDBUS 1)
find_package (Kadu REQUIRED CONFIG)

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

set (CONFIGURATION_FILES
	configuration/freedesktop_notify.ui
)

kadu_plugin (freedesktop_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
