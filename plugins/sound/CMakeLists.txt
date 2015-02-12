project (sound)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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
	sound-player.cpp
	sound-plugin.cpp
	sound-theme-manager.cpp
)

set (CONFIGURATION_FILES
	configuration/sound.ui
)

kadu_plugin (sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_ADDITIONAL_QT_MODULES Multimedia
)
