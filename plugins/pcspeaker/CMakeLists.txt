project (pcspeaker)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	pcspeaker.cpp
	pcspeaker_configuration_widget.cpp
)

set (MOC_SOURCES
	pcspeaker.h
	pcspeaker_configuration_widget.h
)

#set (CONFIGURATION_FILES
#	configuration/pcspeaker.ui
#)

set (LIBRARIES "")

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED COMPONENTS X11)
	if (NOT X11_FOUND)
		message (SEND_ERROR "Could NOT find X11")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	set (LIBRARIES ${X11_X11_LIB})
endif ()

kadu_plugin (pcspeaker
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
