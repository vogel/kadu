set (TRANSLATION_SOURCES
	kadu_en.ts
	kadu_cs.ts
	kadu_de.ts
	kadu_es_ES.ts
	kadu_fr.ts
	kadu_it.ts
	kadu_pl.ts
	kadu_ru.ts
	kadu_sk.ts
)

set (LANGAUGE_FILES
	cs.language
	de.language
	en.language
	pl.language
	ru.language
)

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

add_custom_target (translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu translations)

install (FILES ${TRANSLATION_FILES} ${LANGAUGE_FILES}
	DESTINATION ${KADU_DATA_DIR}/translations
)

if (INSTALL_SDK)
	install (PROGRAMS plugintsupdate.sh DESTINATION ${KADU_PLUGINS_SDK_DIR})
	install (FILES configuration-ui.xsl DESTINATION ${KADU_PLUGINS_SDK_DIR})
endif (INSTALL_SDK)
