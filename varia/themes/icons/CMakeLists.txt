exec_program (./get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTDOWNLOAD}
	OUTPUT_VARIABLE ICON_THEMES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${ICON_THEMES}
	OUTPUT_VARIABLE INSTALL_ICON_THEMES)

message (STATUS "Found icon themes: " ${ICON_THEMES})
message (STATUS "Will install icon themes: " ${INSTALL_ICON_THEMES})

add_custom_target (icon_themes
	DEPENDS ${INSTALL_ICON_THEMES})

add_custom_command (OUTPUT ${INSTALL_ICON_THEMES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND  ../../scripts/autodownload icon ${INSTALL_ICON_THEMES}
	COMMENT "Downloading icon themes ${INSTALL_ICON_THEMES}"
)

add_dependencies (kadu icon_themes)

install (DIRECTORY ${INSTALL_ICON_THEMES}
	DESTINATION share/kadu/themes/icons
	PATTERN ".svn" EXCLUDE
	PATTERN "(README|icons.conf|*.png)"
)
