set (SOURCES
	bmpx.cpp
)

set (MOC_SOURCES
	bmpx.h
)

kadu_module_desc (bmpx_mediaplayer.desc)
qt_wrap_cpp (bmpx_mediaplayer MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (bmpx_mediaplayer ${bmpx_mediaplayer} ${SOURCES} ${MOC_FILES})

if (bmpx_mediaplayer STREQUAL STATIC)
	set_target_properties(bmpx_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (bmpx_mediaplayer STREQUAL STATIC)
	install (TARGETS bmpx_mediaplayer DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (bmpx_mediaplayer STREQUAL STATIC)
