set (SOURCES
	audacious.cpp
)

set (MOC_SOURCES
	audacious.h
)

kadu_module_desc (audacious_mediaplayer.desc)
qt_wrap_cpp (audacious_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (audacious_mediaplayer ${audacious_mediaplayer} ${SOURCES} ${MOC_FILES})

#target_link_libraries (audacious_mediaplayer -lQtDbus)

if (audacious_mediaplayer STREQUAL STATIC)
	set_target_properties(audacious_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (audacious_mediaplayer STREQUAL STATIC)
	install (TARGETS audacious_mediaplayer DESTINATION ${MODULEDIR})
endif (audacious_mediaplayer STREQUAL STATIC)
