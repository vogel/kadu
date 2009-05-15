if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_Xscreensaver_FOUND)
      		MESSAGE(STATUS "Found X11 Screen Saver extension: ${X11_Xscreensaver_LIB}")
	else (X11_Xscreensaver_FOUND)
      		MESSAGE(FATAL_ERROR "Could not find X11 Screen Saver extension")
	endif (X11_Xscreensaver_FOUND)
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

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (idle.desc)
qt_wrap_cpp (idle MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (idle ${idle} ${SOURCES} ${MOC_FILES})

if (UNIX AND NOT APPLE)
	set_target_properties(idle PROPERTIES LINK_FLAGS "-lXss")
endif (UNIX AND NOT APPLE)

if (idle STREQUAL STATIC)
	set_target_properties(idle PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (idle STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(idle kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(idle PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS idle DESTINATION ${MODULEDIR})
endif (idle STREQUAL STATIC)
