exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTODOWNLOAD}
	OUTPUT_VARIABLE ICON_THEMES)

exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${ICON_THEMES}
	OUTPUT_VARIABLE INSTALL_ICON_THEMES)

if (INSTALL_ICON_THEMES)
	add_custom_target (icon_themes)

	foreach (INSTALL_ICON_THEME ${INSTALL_ICON_THEMES})
		message (STATUS "Icon theme: " ${INSTALL_ICON_THEME})
		add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_ICON_THEME}
			COMMAND  ../../scripts/autodownload ARGS icon ${INSTALL_ICON_THEME}
			COMMENT "Downloading icon theme ${INSTALL_ICON_THEME}"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	endforeach (INSTALL_ICON_THEME)

	install (DIRECTORY ${INSTALL_ICON_THEMES}
		DESTINATION share/kadu/themes/icons
		PATTERN ".autodownloaded" EXCLUDE
		PATTERN ".svn" EXCLUDE
		PATTERN "README"
		PATTERN "icons.conf"
		PATTERN "*.png"
	)

	add_dependencies (kadu icon_themes)
endif (INSTALL_ICON_THEMES)
