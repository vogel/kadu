set (SOUND_THEMES
	default
)

if (SOUND_THEMES)
	add_custom_target (sound_themes
		DEPENDS ${SOUND_THEMES}
	)
	add_dependencies (kadu sound_themes)

	foreach (SOUND_THEME ${SOUND_THEMES})
		message (STATUS "Sound theme: " ${SOUND_THEME})
	endforeach (SOUND_THEME)

	install (DIRECTORY ${SOUND_THEMES}
		DESTINATION "${KADU_DATADIR}/themes/sounds"
		FILES_MATCHING
		PATTERN "*.wav"
		PATTERN "sound.conf"
	)
endif (SOUND_THEMES)
