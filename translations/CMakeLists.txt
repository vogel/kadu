set (TRANSLATION_FILES
	kadu_de.qm
	kadu_en.qm
	kadu_fr.qm
	kadu_it.qm
	kadu_pl.qm
	qt_de.qm
	qt_en.qm
	qt_fr.qm
	qt_it.qm
	qt_pl.qm
)

add_custom_target (translations
	DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu translations)

add_custom_command (OUTPUT ${TRANSLATION_FILES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND  lrelease ${TRANSLATION_FILES}
	COMMENT "Compiling translation files ${TRANSLATION_FILES}"
)

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/translations")
