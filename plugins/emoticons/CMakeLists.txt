project (emoticons)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/emoticons
	configuration
	expander
	gui
	theme
	walker
	.
)

set (SOURCES
	configuration/emoticon-configuration.cpp
	configuration/emoticon-configurator.cpp

	expander/animated-emoticon-path-provider.cpp
	expander/emoticon-expander.cpp
	expander/emoticon-expander-dom-visitor-provider.cpp
	expander/static-emoticon-path-provider.cpp

	gui/emoticons-configuration-ui-handler.cpp
	gui/emoticon-clipboard-html-transformer.cpp
	gui/emoticon-selector.cpp
	gui/emoticon-selector-button.cpp
	gui/emoticon-selector-button-popup.cpp
	gui/insert-emoticon-action.cpp

	theme/emoticon-theme.cpp
	theme/emoticon-theme-manager.cpp
	theme/gadu-emoticon-parser.cpp
	theme/gadu-emoticon-theme-loader.cpp

	walker/emoticon-prefix-tree.cpp
	walker/emoticon-prefix-tree-builder.cpp
	walker/emoticon-walker.cpp

	emoticon.cpp
	emoticons-plugin.cpp
)

set (MOC_SOURCES
	configuration/emoticon-configurator.h

	expander/emoticon-expander-dom-visitor-provider.h

	gui/emoticon-selector.h
	gui/emoticon-selector-button.h
	gui/emoticon-selector-button-popup.h
	gui/emoticons-configuration-ui-handler.h
	gui/insert-emoticon-action.h

	theme/emoticon-theme-manager.h

	emoticons-plugin.h
)

set (CONFIGURATION_FILES
	data/configuration/emoticons.ui
)

kadu_plugin (emoticons
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

add_subdirectory (data/themes)
