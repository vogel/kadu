set (EMOTICON_THEMES
	penguins
	tango
)

if (EMOTICON_THEMES)
	add_custom_target (emoticons_themes
		DEPENDS ${EMOTICON_THEMES}
	)

	foreach (EMOTICON_THEME ${EMOTICON_THEMES})
		message (STATUS "Emoticon theme: " ${EMOTICON_THEME})
	endforeach (EMOTICON_THEME)

	add_dependencies (kadu emoticons_themes)

	install (DIRECTORY ${EMOTICON_THEMES}
		DESTINATION ${KADU_DATA_DIR}/themes/emoticons
		PATTERN ".svn" EXCLUDE
		PATTERN "readme.txt"
		PATTERN "emots.txt"
		PATTERN "licence.txt"
		PATTERN "*.png"
		PATTERN "*.gif"
	)
endif (EMOTICON_THEMES)
