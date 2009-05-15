set (SOURCES
	player_external.cpp
)

set (MOC_SOURCES
	player_external.h
)

set (TRANSLATION_SOURCES
	translations/ext_sound_de.ts
	translations/ext_sound_fr.ts
	translations/ext_sound_it.ts
	translations/ext_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/ext_sound.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (ext_sound.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (ext_sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (ext_sound ${ext_sound} ${SOURCES} ${MOC_FILES})
add_custom_target (ext_sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (ext_sound ext_sound-translations)
if (ext_sound STREQUAL STATIC)
	set_target_properties(ext_sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (ext_sound STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(ext_sound kadu_core sound)
	endif (WIN32)
	if (APPLE)
		set_target_properties(ext_sound PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS ext_sound DESTINATION ${MODULEDIR})
endif (ext_sound STREQUAL STATIC)

