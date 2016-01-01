project (history)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/history
	actions
	gui/widgets
	gui/windows
	gui
	model
	search
	storage
	.
)

set (SOURCES
	actions/show-history-action-description.cpp

	gui/widgets/chat-history-tab.cpp
	gui/widgets/history-buddy-configuration-widget.cpp
	gui/widgets/history-buddy-configuration-widget-factory.cpp
	gui/widgets/history-chat-configuration-widget.cpp
	gui/widgets/history-chat-configuration-widget-factory.cpp
	gui/widgets/history-messages-tab.cpp
	gui/widgets/history-tab.cpp
	gui/widgets/history-talkable-combo-box.cpp
	gui/widgets/search-tab.cpp
	gui/widgets/timeline-chat-messages-view.cpp

	gui/windows/history-window.cpp

	model/history-query-results-model.cpp
	model/history-query-results-proxy-model.cpp

	search/history-search-parameters.cpp
	storage/history-messages-storage.cpp
	storage/history-storage.cpp

	buddy-history-delete-handler.cpp
	chats-buddies-splitter.cpp
	history.cpp
	history-messages-prepender.cpp
	history-module.cpp
	history-plugin-injector-factory.cpp
	history-plugin-object.cpp
	history-query.cpp
	history-query-result.cpp
	history-save-thread.cpp
)

set (CONFIGURATION_FILES
	configuration/history.ui
)

kadu_plugin (history
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
