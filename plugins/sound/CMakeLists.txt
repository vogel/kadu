project (sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

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

set (CONFIGURATION_FILES
	configuration/sound.ui
)

kadu_plugin (sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_BUILDDEF SOUND_BUILD
)
