set (SOURCES
	kadu_speech.cpp
)

set (MOC_SOURCES
	kadu_speech.h
)

set (TRANSLATION_SOURCES
	translations/kadu_speech_de.ts
	translations/kadu_speech_ft.ts
	translations/kadu_speech_it.ts
	translations/kadu_speech_pl.ts
)

set (CONFIGURATION_FILES
	configuration/kadu_speech.ui)

kadu_module_desc (kadu_speech.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (kadu_speech MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (kadu_speech ${kadu_speech} ${SOURCES} ${MOC_FILES})
add_custom_target (kadu_speech-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu_speech kadu_speech-translations)

install (TARGETS kadu_speech DESTINATION lib/kadu/modules)
