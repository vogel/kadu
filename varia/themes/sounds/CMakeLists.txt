set (SOUND_THEMES
	default
)

if (SOUND_THEMES)
	add_custom_target (sound_themes
		DEPENDS ${SOUND_THEMES}
	)

	foreach (SOUND_THEME ${SOUND_THEMES})
		message (STATUS "Sound theme: " ${SOUND_THEME})
	endforeach (SOUND_THEME)

	add_dependencies (kadu sound_themes)

	install (DIRECTORY ${SOUND_THEMES}
		DESTINATION ${KADU_DATA_DIR}/themes/sounds
		PATTERN ".svn" EXCLUDE
		PATTERN "*.wav"
		PATTERN "*.txt"
		PATTERN "sound.conf"
	)
endif (SOUND_THEMES)
