set (ICON_THEMES
	default
	glass
	oxygen
)

if (ICON_THEMES)
	add_custom_target (icon_themes
		DEPENDS ${ICON_THEMES}
	)
	add_dependencies (kadu icon_themes)

	foreach (ICON_THEME ${ICON_THEMES})
		message (STATUS "Icon theme: " ${ICON_THEME})
	endforeach (ICON_THEME)

	install (DIRECTORY ${ICON_THEMES}
		DESTINATION "${KADU_DATADIR}/themes/icons"
		FILES_MATCHING
		PATTERN "*.gif"
		PATTERN "*.png"
		PATTERN "*.svg"
		PATTERN "*.svgz"
		# CMake has no option to exclude empty dirs (empty because of pattern matching above)
		PATTERN "src" EXCLUDE
	)
endif (ICON_THEMES)
