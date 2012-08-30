file (GLOB TRANSLATION_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_PROJECT_NAME}_*.ts")
file (GLOB LANGAUGE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.language")

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

add_custom_target (translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu translations)

install (FILES ${TRANSLATION_FILES} ${LANGAUGE_FILES}
	DESTINATION "${KADU_DATADIR}/translations"
)

if (INSTALL_SDK)
	install (PROGRAMS plugintsupdate.sh DESTINATION "${KADU_SDK_DIR}/translations")
	install (FILES configuration-ui.xsl DESTINATION "${KADU_SDK_DIR}/translations")
endif (INSTALL_SDK)
