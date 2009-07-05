exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTODOWNLOAD}
	OUTPUT_VARIABLE EMOTICON_THEMES)

exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${EMOTICON_THEMES}
	OUTPUT_VARIABLE INSTALL_EMOTICON_THEMES)

if (INSTALL_EMOTICON_THEMES)
	add_custom_target (emoticons_themes)

	foreach (INSTALL_EMOTICON_THEME ${INSTALL_EMOTICON_THEMES})
		message (STATUS "Emoticon theme: " ${INSTALL_EMOTICON_THEME})
		add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_EMOTICON_THEME}
			COMMAND  ../../scripts/autodownload emoticon ${INSTALL_EMOTICON_THEME}
			COMMENT "Downloading emoticons theme ${INSTALL_EMOTICON_THEME}"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	endforeach (INSTALL_EMOTICON_THEME)

	add_dependencies (kadu emoticons_themes)

	install (DIRECTORY ${INSTALL_EMOTICON_THEMES}
		DESTINATION share/kadu/themes/emoticons
		PATTERN ".autodownloaded" EXCLUDE
		PATTERN ".svn" EXCLUDE
		PATTERN "readme.txt"
		PATTERN "emots.txt"
		PATTERN "*.png"
		PATTERN "*.gif"
	)
endif (INSTALL_EMOTICON_THEMES)
