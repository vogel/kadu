set (SOURCES
	itunes.cpp
	itunescontroller.cpp
)

set (MOC_SOURCES
	itunes.h
)

kadu_module_desc (itunes_mediaplayer.desc)
qt_wrap_cpp (itunes_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (itunes_mediaplayer ${itunes_mediaplayer} ${SOURCES} ${MOC_FILES})
if (itunes_mediaplayer STREQUAL STATIC)
	set_target_properties(itunes_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (itunes_mediaplayer STREQUAL STATIC)
	if (APPLE)
		set_target_properties(itunes_mediaplayer PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS itunes_mediaplayer DESTINATION ${MODULEDIR})
endif (itunes_mediaplayer STREQUAL STATIC)
