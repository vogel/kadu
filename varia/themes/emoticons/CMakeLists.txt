exec_program (./get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTDOWNLOAD}
	OUTPUT_VARIABLE EMOTICONS_THEMES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${EMOTICONS_THEMES}
	OUTPUT_VARIABLE INSTALL_EMOTICON_THEMES)

message (STATUS "Found emoticon themes: " ${EMOTICONS_THEMES})
message (STATUS "Will install emoticon themes: " ${INSTALL_EMOTICON_THEMES})

add_custom_target (emoticons_themes
	DEPENDS ${INSTALL_EMOTICON_THEMES})

add_custom_command (OUTPUT ${INSTALL_EMOTICON_THEMES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND  ../../scripts/autodownload emoticon ${INSTALL_EMOTICON_THEMES}
	COMMENT "Downloading emoticons themes ${INSTALL_EMOTICON_THEMES}"
)

add_dependencies (kadu emoticons_themes)
