set (SOURCES
	falf.cpp
)

set (MOC_SOURCES
	falf.h
)

kadu_module_desc (falf_mediaplayer.desc)
qt_wrap_cpp (falf_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

#pkg_search_module (FALF REQUIRED falf)

add_library (falf_mediaplayer ${falf_mediaplayer} ${SOURCES} ${MOC_FILES})

#target_link_libraries (falf_mediaplayer ${FALF_LIBRARIES})
#include_directories (${FALF_INCLUDE_DIRS})

if (falf_mediaplayer STREQUAL STATIC)
	set_target_properties(falf_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (falf_mediaplayer STREQUAL STATIC)
	install (TARGETS falf_mediaplayer DESTINATION ${MODULEDIR})
endif (falf_mediaplayer STREQUAL STATIC)

