project (idle)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

kadu_api_directories (plugins/idle
	.
)

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED COMPONENTS X11 Xutil Xscreensaver)
	if (X11_Xscreensaver_FOUND)
		message (STATUS "Found required X11 libraries: ${X11_X11_LIB};${X11_Xscreensaver_LIB}")
	else (X11_Xscreensaver_FOUND)
		message (SEND_ERROR "Could NOT find X11 XScrnSaver")
	endif (X11_Xscreensaver_FOUND)

	include_directories (${X11_INCLUDE_DIR})
	set (LIBRARIES ${X11_X11_LIB} ${X11_Xscreensaver_LIB})

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
