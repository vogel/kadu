set (SOURCES
	amarok2.cpp
	amarok2controller.cpp
)

set (MOC_SOURCES
	amarok2.h
	amarok2controller.h
)

kadu_module_desc (amarok2_mediaplayer.desc)
qt_wrap_cpp (amarok2_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (amarok2_mediaplayer ${amarok2_mediaplayer} ${SOURCES} ${MOC_FILES})

#target_link_libraries (amarok2_mediaplayer -lQtDbus)

if (amarok2_mediaplayer STREQUAL STATIC)
	set_target_properties(amarok2_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (amarok2_mediaplayer STREQUAL STATIC)
	install (TARGETS amarok2_mediaplayer DESTINATION lib/kadu/modules)
endif (amarok2_mediaplayer STREQUAL STATIC)
