set (SOURCES
	qt4_sound.cpp
)

set (MOC_SOURCES
	qt4_sound.h
)

kadu_module_desc (qt4_sound.desc)
qt_wrap_cpp (qt4_sound MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (qt4_sound ${qt4_sound} ${SOURCES} ${MOC_FILES})
if (qt4_sound STREQUAL STATIC)
	set_target_properties(qt4_sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (qt4_sound STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(qt4_sound kadu_core sound)
	endif (WIN32)
	if (APPLE)
		set_target_properties(qt4_sound PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS qt4_sound DESTINATION lib/kadu/modules)
endif (qt4_sound STREQUAL STATIC)

