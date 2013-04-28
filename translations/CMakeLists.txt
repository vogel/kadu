option (INSTALL_UNOFFICIAL_TRANSLATIONS "Install translations generated from all *.ts files, not only the ones enabled by *.language files. In kadu-core this also means generating additional *.language files so that the unofficial translations can be used." OFF)
set (KADU_INSTALL_UNOFFICIAL_TRANSLATIONS ${INSTALL_UNOFFICIAL_TRANSLATIONS} PARENT_SCOPE) # Do not use it here, it's in parent scope

if (INSTALL_UNOFFICIAL_TRANSLATIONS)
	file (GLOB LANGUAGE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.language")
	file (GLOB TRANSLATION_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "kadu_*.ts")

	foreach (TRANSLATION_SOURCE ${TRANSLATION_SOURCES})
		get_filename_component (LANGUAGE "${TRANSLATION_SOURCE}" NAME_WE)
		string (REGEX REPLACE "^kadu_" "" LANGUAGE ${LANGUAGE})
		list (FIND LANGUAGE_FILES "${LANGUAGE}.language" LANGUAGE_FOUND)
		if (LANGUAGE_FOUND EQUAL -1)
			set (file_ "${CMAKE_CURRENT_BINARY_DIR}/${LANGUAGE}.language")
			file (WRITE "${file_}" "${LANGUAGE}\n")
			list (APPEND LANGUAGE_FILES "${file_}")
		endif ()
	endforeach ()
else ()
	file (GLOB LANGUAGE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.language")

	set (LANGUAGES)
	set (TRANSLATION_SOURCES)
	foreach (LANGUAGE_FILE ${LANGUAGE_FILES})
		get_filename_component (LANGUAGE "${LANGUAGE_FILE}" NAME_WE)
		list (APPEND LANGUAGES ${LANGUAGE})
		set (file_ "${CMAKE_CURRENT_SOURCE_DIR}/kadu_${LANGUAGE}.ts")
		if (EXISTS "${file_}")
			list (APPEND TRANSLATION_SOURCES "${file_}")
		endif ()
	endforeach ()

	list (SORT LANGUAGES)
	set (KADU_ENABLED_LANGUAGES ${LANGUAGES} PARENT_SCOPE) # Do not use it here, it's in parent scope
endif ()

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

add_custom_target (translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kadu translations)

install (FILES ${TRANSLATION_FILES} ${LANGUAGE_FILES}
	DESTINATION "${KADU_INSTALL_DATA_DIR}/translations"
)

if (KADU_INSTALL_SDK)
	install (PROGRAMS plugintsupdate.sh DESTINATION "${KADU_INSTALL_SDK_DIR}/translations")
	install (FILES configuration-ui.xsl DESTINATION "${KADU_INSTALL_SDK_DIR}/translations")
endif ()
