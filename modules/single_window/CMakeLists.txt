set (SOURCES
	single_window.cpp
)

set (MOC_SOURCES
	single_window.h
)

kadu_module_desc (single_window.desc)
qt_wrap_cpp (single_window MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (single_window ${single_window} ${SOURCES} ${MOC_FILES})
if (single_window STREQUAL STATIC)
	set_target_properties(single_window PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (single_window STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(single_window kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(single_window PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS single_window DESTINATION ${MODULEDIR})
endif (single_window STREQUAL STATIC)

