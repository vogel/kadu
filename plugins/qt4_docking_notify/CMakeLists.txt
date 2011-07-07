project (qt4_docking_notify)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	qt4-docking-notify.cpp
	qt4-docking-notify-configuration-widget.cpp
	qt4-docking-notify-plugin.cpp
)

set (MOC_SOURCES
	qt4-docking-notify.h
	qt4-docking-notify-configuration-widget.h
	qt4-docking-notify-plugin.h
)

set (TRANSLATION_SOURCES
	translations/qt4_docking_notify_cs.ts
	translations/qt4_docking_notify_de.ts
	translations/qt4_docking_notify_en.ts
	translations/qt4_docking_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/qt4-docking-notify.ui
)

kadu_plugin (qt4_docking_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES qt4_docking
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
