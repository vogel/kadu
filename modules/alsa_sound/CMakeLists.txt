set (SOURCES
	alsa_sound.cpp
)

set (MOC_SOURCES
	alsa_sound.h
)

set (TRANSLATION_SOURCES
	translations/alsa_sound_de.ts
	translations/alsa_sound_fr.ts
	translations/alsa_sound_it.ts
	translations/alsa_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/alsa_sound.ui)

kadu_module_desc (alsa_sound.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (alsa_sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (ALSA REQUIRED alsa)

add_library (alsa_sound ${alsa_sound} ${SOURCES} ${MOC_FILES})
add_custom_target (alsa_sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (alsa_sound alsa_sound-translations)

target_link_libraries (alsa_sound ${ALSA_LIBRARIES})
include_directories (${ALSA_INCLUDE_DIRS})

install (TARGETS alsa_sound DESTINATION lib/kadu/modules)
