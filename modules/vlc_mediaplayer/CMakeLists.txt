set (SOURCES
	vlc.cpp
)

set (MOC_SOURCES
	vlc.h
)

kadu_module_desc (vlc_mediaplayer.desc)
qt_wrap_cpp (vlc_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (vlc_mediaplayer ${vlc_mediaplayer} ${SOURCES} ${MOC_FILES})

target_link_libraries (vlc_mediaplayer ${QT_QTDBUS_LIBRARY})

if (vlc_mediaplayer STREQUAL STATIC)
	set_target_properties(vlc_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (vlc_mediaplayer STREQUAL STATIC)
	install (TARGETS vlc_mediaplayer DESTINATION ${MODULEDIR})
endif (vlc_mediaplayer STREQUAL STATIC)
