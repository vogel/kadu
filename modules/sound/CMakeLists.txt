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

kadu_module_desc (sound.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (sound ${sound} ${SOURCES} ${MOC_FILES})
add_custom_target (sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (sound sound-translations)

install (TARGETS sound DESTINATION lib/kadu/modules)
