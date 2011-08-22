project (idle)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

kadu_api_directories (plugins/idle
	.
)

if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_Xscreensaver_FOUND)
		message (STATUS "Found X11 Screen Saver extension: ${X11_Xscreensaver_LIB}")
	else (X11_Xscreensaver_FOUND)
		message (FATAL_ERROR "Could not find X11 Screen Saver extension")
	endif (X11_Xscreensaver_FOUND)

	include_directories (${X11_INCLUDE_DIR})
	set (LIBRARIES ${X11_Xscreensaver_LIB})

	set (SOURCES
		idle-plugin.cpp
		idle_x11.cpp
	)
endif (UNIX AND NOT APPLE)

if (APPLE)
	set (SOURCES
		idle-plugin.cpp
		idle_mac.cpp
	)
endif (APPLE)

if (WIN32)
	set (SOURCES
		idle-plugin.cpp
		idle_win.cpp
	)
endif (WIN32)

set (MOC_SOURCES
	idle.h
	idle-plugin.h
)

kadu_plugin (idle
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_BUILDDEF IDLE_BUILD
)
