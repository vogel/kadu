set (SOURCES
	xmms.cpp
)

set (MOC_SOURCES
	xmms.h
)

kadu_module_desc (xmms_mediaplayer.desc)
qt_wrap_cpp (xmms_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (GLIB REQUIRED glib)

set (XMMS_LIBRARIES
)

set (XMMS_INCLUDE_DIRS
)

add_library (xmms_mediaplayer ${xmms_mediaplayer} ${SOURCES} ${MOC_FILES})

target_link_libraries (xmms_mediaplayer ${XMMS_LIBRARIES} ${GLIB_LIBRARIES})
include_directories (${XMMS_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})

set_target_properties(xmms_mediaplayer PROPERTIES LINK_FLAGS "-lxmms" )

if (xmms_mediaplayer STREQUAL STATIC)
	set_target_properties(xmms_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (xmms_mediaplayer STREQUAL STATIC)
	install (TARGETS xmms_mediaplayer DESTINATION ${MODULEDIR})
endif (xmms_mediaplayer STREQUAL STATIC)

