set (SOURCES
	amarok.cpp
)

set (MOC_SOURCES
	amarok.h
)

kadu_module_desc (amarok1_mediaplayer.desc)
qt_wrap_cpp (amarok1_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (amarok1_mediaplayer ${amarok1_mediaplayer} ${SOURCES} ${MOC_FILES})

if (amarok1_mediaplayer STREQUAL STATIC)
	set_target_properties(amarok1_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (amarok1_mediaplayer STREQUAL STATIC)
	install (TARGETS amarok1_mediaplayer DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (amarok1_mediaplayer STREQUAL STATIC)
