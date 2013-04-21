file (GLOB TRANSLATION_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "kadu_*.ts")
file (GLOB LANGAUGE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.language")

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

add_custom_target (translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu translations)

install (FILES ${TRANSLATION_FILES} ${LANGAUGE_FILES}
	DESTINATION "${KADU_INSTALL_DATA_DIR}/translations"
)

if (KADU_INSTALL_SDK)
	install (PROGRAMS plugintsupdate.sh DESTINATION "${KADU_INSTALL_SDK_DIR}/translations")
	install (FILES configuration-ui.xsl DESTINATION "${KADU_INSTALL_SDK_DIR}/translations")
endif ()
