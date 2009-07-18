set (TRANSLATION_SOURCES
	kadu_de.ts
	kadu_en.ts
	kadu_fr.ts
	kadu_it.ts
	kadu_pl.ts
	qt_de.ts
	qt_en.ts
	qt_fr.ts
	qt_it.ts
	qt_pl.ts
)

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

add_custom_target (translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu translations)

install (FILES ${TRANSLATION_FILES}
	DESTINATION ${KADU_DATA_DIR}/translations)
