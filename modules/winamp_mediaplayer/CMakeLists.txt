set (SOURCES
	winamp.cpp
)

set (MOC_SOURCES
	winamp.h
)

if (WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif (WIN32)

kadu_module_desc (winamp_mediaplayer.desc)
qt_wrap_cpp (winamp_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)


add_library (winamp_mediaplayer ${winamp_mediaplayer} ${SOURCES} ${MOC_FILES})

if (winamp_mediaplayer STREQUAL STATIC)
	set_target_properties(winamp_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (winamp_mediaplayer STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(winamp_mediaplayer kadu_core mediaplayer)
	endif (WIN32)
	if (APPLE)
		set_target_properties(winamp_mediaplayer PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS winamp_mediaplayer DESTINATION ${MODULEDIR})
endif (winamp_mediaplayer STREQUAL STATIC)

