if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_Xscreensaver_FOUND)
		message (STATUS "Found X11 Screen Saver extension: ${X11_Xscreensaver_LIB}")
	else (X11_Xscreensaver_FOUND)
		message (FATAL_ERROR "Could not find X11 Screen Saver extension")
	endif (X11_Xscreensaver_FOUND)

	set (LIBRARIES ${X11_Xscreensaver_LIB})

	set (SOURCES
		idle.cpp
		idle_x11.cpp
	)
endif (UNIX AND NOT APPLE)

if (APPLE)
	set (SOURCES
		idle.cpp
		idle_mac.cpp
	)
endif (APPLE)

if (WIN32)
	set (SOURCES
		idle.cpp
		idle_win.cpp
	)
endif (WIN32)

set (MOC_SOURCES
	idle.h
)

kadu_module (idle
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_LIBRARIES ${LIBRARIES}
)

if (WIN32)
	set_target_properties (idle PROPERTIES COMPILE_FLAGS "-DIDLE_BUILD")
endif (WIN32)
