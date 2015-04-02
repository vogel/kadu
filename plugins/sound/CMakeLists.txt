project (sound)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/sound
	gui
	notification
	.
)

set (SOURCES
	gui/sound-actions.cpp
	gui/sound-buddy-configuration-widget.cpp
	gui/sound-buddy-configuration-widget-factory.cpp
	gui/sound-chat-configuration-widget.cpp
	gui/sound-chat-configuration-widget-factory.cpp
	gui/sound-configuration-ui-handler.cpp
	gui/sound-configuration-widget.cpp
	gui/sound-mute-action.cpp
	gui/sound-select-file.cpp
	notification/sound-notifier.cpp
	sound-manager.cpp
	sound-module.cpp
	sound-player.cpp
	sound-plugin.cpp
	sound-theme-manager.cpp
)

set (CONFIGURATION_FILES
	data/configuration/sound.ui
)

kadu_plugin (sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_ADDITIONAL_QT_MODULES Multimedia
)
