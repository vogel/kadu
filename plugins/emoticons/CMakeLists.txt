project (emoticons)

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

set (SOURCES
	animated-emoticon-path-provider.cpp
	emoticon.cpp
	emoticon-expander.cpp
	emoticon-expander-dom-visitor-provider.cpp
	emoticon-prefix-tree.cpp
	emoticon-prefix-tree-builder.cpp
	emoticon-selector-button.cpp
	emoticon-selector.cpp
	emoticon-theme.cpp
	emoticon-theme-manager.cpp
	emoticon-walker.cpp
	emoticons-configuration-ui-handler.cpp
	emoticons-manager.cpp
	emoticons-plugin.cpp
	gadu-emoticon-parser.cpp
	gadu-emoticon-theme-loader.cpp
	insert-emoticon-action.cpp
	static-emoticon-path-provider.cpp
)

set (MOC_SOURCES
	emoticon-selector-button.h
	emoticon-selector.h
	emoticon-theme-manager.h
	emoticons-configuration-ui-handler.h
	emoticons-manager.h
	emoticons-plugin.h
	insert-emoticon-action.h
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
