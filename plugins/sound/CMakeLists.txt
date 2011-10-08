project (sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

include (FindPkgConfig)

kadu_api_directories (plugins/sound
	configuration/gui
	configuration
	gui/widgets
	gui
	.
)

set (SOURCES
	configuration/gui/sound-configuration-ui-handler.cpp
	gui/widgets/configuration/sound-configuration-widget.cpp
	notify/sound-notifier.cpp
	sound-actions.cpp
	sound-file.cpp
	sound-manager.cpp
	sound-play-thread.cpp
	sound-plugin.cpp
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
	sound-plugin.h
)

set (TRANSLATION_SOURCES
	translations/sound_cs.ts
	translations/sound_de.ts
	translations/sound_en.ts
	translations/sound_it.ts
	translations/sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sound.ui
)

if (WIN32)
	if (NOT WIN_LIBSNDFILE_DIR)
		message (SEND_ERROR "Pass path to libsndfile -DWIN_LIBSNDFILE_DIR=path")
	else (NOT WIN_LIBSNDFILE_DIR)
		set (SNDFILE_INCLUDE_DIRS ${WIN_LIBSNDFILE_DIR}/include)

		if (MSVC)
			set (SNDFILE_LIBRARY_DIRS ${WIN_LIBSNDFILE_DIR}/lib)
			set (SNDFILE_LIBRARIES libsndfile-1)
		else (MSVC)
			set (SNDFILE_LIBRARY_DIRS ${WIN_LIBSNDFILE_DIR}/bin)
			set (SNDFILE_LIBRARIES sndfile-1)
		endif (MSVC)
	endif (NOT WIN_LIBSNDFILE_DIR)
else (WIN32)
	pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
endif (WIN32)

include_directories (${SNDFILE_INCLUDE_DIRS})
link_directories (${SNDFILE_LIBRARY_DIRS})

kadu_plugin (sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${SNDFILE_LIBRARIES}
	PLUGIN_BUILDDEF SOUND_BUILD
)
