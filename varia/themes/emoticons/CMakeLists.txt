exec_program (./get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTDOWNLOAD}
	OUTPUT_VARIABLE EMOTICONS_THEMES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${EMOTICONS_THEMES}
	OUTPUT_VARIABLE INSTALL_EMOTICON_THEMES)

message (STATUS "Found emoticon themes: " ${EMOTICONS_THEMES})

add_custom_target (emoticons_themes
	DEPENDS ${INSTALL_EMOTICON_THEMES})

foreach (INSTALL_EMOTICON_THEME ${INSTALL_EMOTICON_THEMES})
message (STATUS "Will install emoticon theme: " ${INSTALL_EMOTICON_THEME})
	add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_EMOTICON_THEME}
		COMMAND  ../../scripts/autodownload emoticon ${INSTALL_EMOTICON_THEME}
		COMMENT "Downloading emoticons theme ${INSTALL_EMOTICON_THEME}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
endforeach (INSTALL_EMOTICON_THEME)

add_dependencies (kadu emoticons_themes)

install (DIRECTORY ${INSTALL_EMOTICON_THEMES}
	DESTINATION share/kadu/themes/emoticons
	PATTERN ".svn" EXCLUDE
	PATTERN "readme.txt"
	PATTERN "emots.txt"
	PATTERN "*.png"
	PATTERN "*.gif"
)
