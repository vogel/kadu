project (idle)

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

kadu_api_directories (plugins/idle
	.
)

if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_Xscreensaver_FOUND)
		message (STATUS "Found X11 Screen Saver extension: ${X11_Xscreensaver_LIB}")
	else ()
		message (FATAL_ERROR "Could not find X11 Screen Saver extension")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	set (LIBRARIES ${X11_Xscreensaver_LIB})

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
	PLUGIN_BUILDDEF IDLE_BUILD
)
