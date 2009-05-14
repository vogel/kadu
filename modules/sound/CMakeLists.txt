set (SOURCES
	sound.cpp
	sound_file.cpp
	sound_slots.cpp
)

set (MOC_SOURCES
	sound.h
	sound_file.h
	sound_slots.h
)

set (TRANSLATION_SOURCES
	translations/sound_de.ts
	translations/sound_fr.ts
	translations/sound_it.ts
	translations/sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sound.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (sound.desc)
kadu_module_configuration (${CONFIGURATION_FILES})
qt_wrap_cpp (sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
include_directories (${SNDFILE_INCLUDE_DIRS})
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (sound ${sound} ${SOURCES} ${MOC_FILES})
add_custom_target (sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (sound sound-translations)
set_target_properties(sound PROPERTIES COMPILE_FLAGS "-DSOUND_BUILD" )

target_link_libraries (sound ${SNDFILE_LIBRARIES})
include_directories (${SNDFILE_INCLUDE_DIRS})

if (sound STREQUAL STATIC)
	set_target_properties(sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (sound STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(sound kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(sound PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS sound DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (sound STREQUAL STATIC)



