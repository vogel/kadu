project (pcspeaker)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	pcspeaker.cpp
	pcspeaker_configuration_widget.cpp
)

set (MOC_SOURCES
	pcspeaker.h
	pcspeaker_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/pcspeaker_cs.ts
	translations/pcspeaker_de.ts
	translations/pcspeaker_en.ts
	translations/pcspeaker_it.ts
	translations/pcspeaker_pl.ts
)

#set (CONFIGURATION_FILES
#	configuration/pcspeaker.ui
#)

if (UNIX AND NOT APPLE)
	include (FindX11)
	include_directories (${X11_INCLUDE_DIR})
endif (UNIX AND NOT APPLE)

kadu_plugin (pcspeaker
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
