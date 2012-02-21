set (EMOTICON_THEMES
	penguins
	tango
)

if (EMOTICON_THEMES)
	add_custom_target (emoticons_themes
		DEPENDS ${EMOTICON_THEMES}
	)
	add_dependencies (kadu emoticons_themes)

	foreach (EMOTICON_THEME ${EMOTICON_THEMES})
		message (STATUS "Emoticon theme: " ${EMOTICON_THEME})
	endforeach (EMOTICON_THEME)

	install (DIRECTORY ${EMOTICON_THEMES}
		DESTINATION "${KADU_DATADIR}/themes/emoticons"
		FILES_MATCHING
		PATTERN "*.png"
		PATTERN "*.gif"
		PATTERN "emots.txt"
		PATTERN "readme.txt"
		PATTERN "licence.txt"
	)
endif (EMOTICON_THEMES)
