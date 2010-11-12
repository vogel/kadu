if (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE ICON_THEMES
	)
message (STATUS "ICON_THEMES: ${ICON_THEMES}")

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${ICON_THEMES}
		OUTPUT_VARIABLE INSTALL_ICON_THEMES
	)
message (STATUS "INSTALL_ICON_THEMES: ${INSTALL_ICON_THEMES}")
else (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE ICON_THEMES
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${ICON_THEMES}
		OUTPUT_VARIABLE INSTALL_ICON_THEMES
	)
endif (WIN32)

if (INSTALL_ICON_THEMES)
	add_custom_target (icon_themes)

	foreach (INSTALL_ICON_THEME ${INSTALL_ICON_THEMES})
		message (STATUS "Icon theme: " ${INSTALL_ICON_THEME})
		if (ENABLE_AUTODOWNLOAD)
			if (WIN32)
message (STATUS "downloading: ${INSTALL_ICON_THEME}")
				exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload.bat ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS theme ${INSTALL_ICON_THEME}
			)
			else (WIN32)
				exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS theme ${INSTALL_ICON_THEME}
			)
			endif (WIN32)
		endif (ENABLE_AUTODOWNLOAD)
	endforeach (INSTALL_ICON_THEME)

	install (DIRECTORY ${INSTALL_ICON_THEMES}
		DESTINATION ${KADU_DATA_DIR}/themes/icons
		PATTERN ".autodownloaded" EXCLUDE
		PATTERN "README"
		PATTERN "*.png"
		PATTERN "*.svg"
	)

	add_dependencies (kadu icon_themes)
endif (INSTALL_ICON_THEMES)
