exec_program (./get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTDOWNLOAD}
	OUTPUT_VARIABLE SOUND_THEMES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${SOUND_THEMES}
	OUTPUT_VARIABLE INSTALL_SOUND_THEMES)

if (${SOUND_THEMES})
	message (STATUS "Found sound themes: " ${SOUND_THEMES})

	add_custom_target (sound_themes
		DEPENDS ${INSTALL_SOUND_THEMES})

	foreach (INSTALL_SOUND_THEME ${INSTALL_SOUND_THEMES})
		message (STATUS "Will install sound theme: " ${INSTALL_SOUND_THEME})
		add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_SOUND_THEME}
			COMMAND  ../../scripts/autodownload sound ${INSTALL_SOUND_THEME}
			COMMENT "Downloading sound theme ${INSTALL_SOUND_THEMES}"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	endforeach (INSTALL_SOUND_THEME)

	add_dependencies (kadu sound_themes)

	install (DIRECTORY ${INSTALL_SOUND_THEMES}
		DESTINATION share/kadu/themes/sound
		PATTERN ".svn" EXCLUDE
		PATTERN "*.wav"
	)
endif (${SOUND_THEMES})

