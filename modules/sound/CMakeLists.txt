set (SOURCES
	configuration/gui/sound-configuration-ui-handler.cpp
	gui/widgets/configuration/sound-configuration-widget.cpp
	notify/sound-notifier.cpp
	sound-actions.cpp
	sound-file.cpp
	sound-manager.cpp
	sound-module.cpp
	sound-play-thread.cpp
	sound-theme-manager.cpp
)

set (MOC_SOURCES
	configuration/gui/sound-configuration-ui-handler.h
	gui/widgets/configuration/sound-configuration-widget.h
	notify/sound-notifier.h
	sound-actions.h
	sound-manager.h
	sound-play-thread.h
	sound-player.h
)

set (TRANSLATION_SOURCES
	translations/sound_en.ts
	translations/sound_de.ts
	translations/sound_fr.ts
	translations/sound_it.ts
	translations/sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sound.ui
)

kadu_module (sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF SOUND_BUILD
)

if (NOT MINGW)
	pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
else (NOT MINGW)
	if (NOT MINGW_LIBSNDFILE_DIR)
		message (SEND_ERROR " Pass path to libsndfile -DMINGW_LIBSNDFILE_DIR=path")
	else (NOT MINGW_LIBSNDFILE_DIR)
		set (SNDFILE_INCLUDE_DIRS ${MINGW_LIBSNDFILE_DIR}/include)
		set (SNDFILE_LIBRARY_DIRS ${MINGW_LIBSNDFILE_DIR})
		set (SNDFILE_LIBRARIES -lsndfile-1 -L${MINGW_LIBSNDFILE_DIR})
	endif (NOT MINGW_LIBSNDFILE_DIR)
endif (NOT MINGW)

if (WIN32)
	set_target_properties (sound PROPERTIES COMPILE_FLAGS "-DSOUND_BUILD")
endif (WIN32)

include_directories (${SNDFILE_INCLUDE_DIRS})
link_directories (${SNDFILE_LIBRARY_DIRS})
target_link_libraries (sound ${SNDFILE_LIBRARIES})
