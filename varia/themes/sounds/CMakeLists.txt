exec_program (./get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTDOWNLOAD}
	OUTPUT_VARIABLE SOUND_THEMES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${SOUND_THEMES}
	OUTPUT_VARIABLE INSTALL_SOUND_THEMES)

message (STATUS "Found sound themes: " ${SOUND_THEMES})
message (STATUS "Will install sound themes: " ${INSTALL_SOUND_THEMES})

add_custom_target (sound_themes
	DEPENDS ${INSTALL_SOUND_THEMES})

add_custom_command (OUTPUT ${INSTALL_SOUND_THEMES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND  ../../scripts/autodownload sound ${INSTALL_SOUND_THEMES}
	COMMENT "Downloading sound themes ${INSTALL_SOUND_THEMES}"
)

add_dependencies (kadu sound_themes)
