project (idle)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/idle
	.
)

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED COMPONENTS X11 Xutil Xscreensaver)
	if (X11_Xscreensaver_FOUND)
		message (STATUS "Found required X11 libraries: ${X11_X11_LIB};${X11_Xscreensaver_LIB}")
	else ()
		message (SEND_ERROR "Could NOT find X11 XScrnSaver")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	set (LIBRARIES ${X11_X11_LIB} ${X11_Xscreensaver_LIB})

	set (SOURCES
		idle-plugin.cpp
		idle_x11.cpp
	)
endif ()

if (APPLE)
	set (SOURCES
		idle-plugin.cpp
		idle_mac.cpp
	)
endif ()

if (WIN32)
	set (SOURCES
		idle-plugin.cpp
		idle_win.cpp
	)
endif ()

set (MOC_SOURCES
	idle.h
	idle-plugin.h
)

kadu_plugin (idle
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
