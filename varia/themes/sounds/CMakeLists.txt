if (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE SOUND_THEMES
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${SOUND_THEMES}
		OUTPUT_VARIABLE INSTALL_SOUND_THEMES
	)
else (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE SOUND_THEMES
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${SOUND_THEMES}
		OUTPUT_VARIABLE INSTALL_SOUND_THEMES
	)
endif (WIN32)

if (INSTALL_SOUND_THEMES)
	add_custom_target (sound_themes
		DEPENDS ${INSTALL_SOUND_THEMES}
	)

	foreach (INSTALL_SOUND_THEME ${INSTALL_SOUND_THEMES})
		message (STATUS "Sound theme: " ${INSTALL_SOUND_THEME})
		if (ENABLE_AUTODOWNLOAD)
			if (WIN32)
				exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload.bat ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS theme ${INSTALL_SOUND_THEME}
			)
			else (WIN32)
				exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS theme ${INSTALL_SOUND_THEME}
			)
			endif (WIN32)
		endif (ENABLE_AUTODOWNLOAD)
	endforeach (INSTALL_SOUND_THEME)

	add_dependencies (kadu sound_themes)

	install (DIRECTORY ${INSTALL_SOUND_THEMES}
		DESTINATION share/kadu/themes/sounds
		PATTERN ".autodownloaded" EXCLUDE
		PATTERN ".svn" EXCLUDE
		PATTERN "*.wav"
	)
endif (INSTALL_SOUND_THEMES)
