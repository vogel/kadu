set (SOURCES
	dsp_sound.cpp
)

set (MOC_SOURCES
	dsp_sound.h
)

set (TRANSLATION_SOURCES
	translations/dsp_sound_de.ts
	translations/dsp_sound_fr.ts
	translations/dsp_sound_it.ts
	translations/dsp_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/dsp_sound.ui)

kadu_module_desc (dsp_sound.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (dsp_sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (dsp_sound ${dsp_sound} ${SOURCES} ${MOC_FILES})
add_custom_target (dsp_sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (dsp_sound dsp_sound-translations)

if (dsp_sound STREQUAL STATIC)
	set_target_properties(dsp_sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (dsp_sound STREQUAL STATIC)
	install (TARGETS dsp_sound DESTINATION ${MODULEDIR})
endif (dsp_sound STREQUAL STATIC)

