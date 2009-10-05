set (SOURCES
	xmms2.cpp
)

set (MOC_SOURCES
	xmms2.h
)

kadu_module_desc (xmms2_mediaplayer.desc)
qt_wrap_cpp (xmms2_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (xmms2_mediaplayer ${xmms2_mediaplayer} ${SOURCES} ${MOC_FILES})

target_link_libraries (xmms2_mediaplayer ${QT_QTDBUS_LIBRARY})

if (xmms2_mediaplayer STREQUAL STATIC)
	set_target_properties(xmms2_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (xmms2_mediaplayer STREQUAL STATIC)
	install (TARGETS xmms2_mediaplayer DESTINATION ${MODULEDIR})
endif (xmms2_mediaplayer STREQUAL STATIC)
