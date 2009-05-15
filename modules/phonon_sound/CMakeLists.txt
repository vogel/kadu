set (SOURCES
	phonon_sound.cpp
)

set (MOC_SOURCES
	phonon_sound.h
)

kadu_module_desc (phonon_sound.desc)
qt_wrap_cpp (phonon_sound MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (PHONON REQUIRED phonon)
include_directories (${PHONON_INCLUDE_DIRS})
link_directories (${PHONON_LIBRARY_DIRS})

add_library (phonon_sound ${phonon_sound} ${SOURCES} ${MOC_FILES})
target_link_libraries(phonon_sound ${PHONON_LIBRARIES})

if (phonon_sound STREQUAL STATIC)
	set_target_properties(phonon_sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (phonon_sound STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(phonon_sound kadu_core sound)
	endif (WIN32)
	if (APPLE)
		set_target_properties(phonon_sound PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS phonon_sound DESTINATION ${MODULEDIR})
endif (phonon_sound STREQUAL STATIC)

