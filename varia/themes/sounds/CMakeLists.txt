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
	endforeach ()

	install (DIRECTORY ${SOUND_THEMES}
		DESTINATION "${KADU_INSTALL_DATA_DIR}/themes/sounds"
		FILES_MATCHING
		PATTERN "*.wav"
		PATTERN "sound.conf"
	)
endif ()
