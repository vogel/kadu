set (ICON_THEMES
	default
	glass
	oxygen
)

if (ICON_THEMES)
	add_custom_target (icon_themes)

	foreach (ICON_THEME ${ICON_THEMES})
		message (STATUS "Icon theme: " ${ICON_THEME})
	endforeach (ICON_THEME)

	install (DIRECTORY ${ICON_THEMES}
		DESTINATION ${KADU_DATA_DIR}/themes/icons
		PATTERN "README"
		PATTERN "*.png"
		PATTERN "*.svg"
	)

	add_dependencies (kadu icon_themes)
endif (ICON_THEMES)
