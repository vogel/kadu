set (SOURCES
	dragon.cpp
)

set (MOC_SOURCES
	dragon.h
)

kadu_module_desc (dragon_mediaplayer.desc)
qt_wrap_cpp (dragon_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (dragon_mediaplayer ${dragon_mediaplayer} ${SOURCES} ${MOC_FILES})

target_link_libraries (dragon_mediaplayer ${QT_QTDBUS_LIBRARY})

if (dragon_mediaplayer STREQUAL STATIC)
	set_target_properties(dragon_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (dragon_mediaplayer STREQUAL STATIC)
	install (TARGETS dragon_mediaplayer DESTINATION ${MODULEDIR})
endif (dragon_mediaplayer STREQUAL STATIC)
