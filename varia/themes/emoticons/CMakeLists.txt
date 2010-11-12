if (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE EMOTICON_THEMES
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${EMOTICON_THEMES}
		OUTPUT_VARIABLE INSTALL_EMOTICON_THEMES
	)
else (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE EMOTICON_THEMES
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${EMOTICON_THEMES}
		OUTPUT_VARIABLE INSTALL_EMOTICON_THEMES
	)
endif (WIN32)

if (INSTALL_EMOTICON_THEMES)
	add_custom_target (emoticons_themes
		DEPENDS ${INSTALL_EMOTICON_THEMES}
	)

	foreach (INSTALL_EMOTICON_THEME ${INSTALL_EMOTICON_THEMES})
		message (STATUS "Emoticon theme: " ${INSTALL_EMOTICON_THEME})
		if (ENABLE_AUTODOWNLOAD)
			if (WIN32)
				exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload.bat ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS theme ${INSTALL_EMOTICON_THEME}
			)
			else (WIN32)
				exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS theme ${INSTALL_EMOTICON_THEME}
			)
			endif (WIN32)
		endif (ENABLE_AUTODOWNLOAD)
	endforeach (INSTALL_EMOTICON_THEME)

	add_dependencies (kadu emoticons_themes)

	install (DIRECTORY ${INSTALL_EMOTICON_THEMES}
		DESTINATION share/kadu/themes/emoticons
		PATTERN ".autodownloaded" EXCLUDE
		PATTERN ".svn" EXCLUDE
		PATTERN "readme.txt"
		PATTERN "emots.txt"
		PATTERN "licence.txt"
		PATTERN "*.png"
		PATTERN "*.gif"
	)
endif (INSTALL_EMOTICON_THEMES)
