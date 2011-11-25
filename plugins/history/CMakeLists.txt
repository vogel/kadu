project (history)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

kadu_api_directories (plugins/history
	actions
	gui/windows
	gui
	model
	search
	storage
	.
)

set (SOURCES
	actions/show-history-action-description.cpp

	gui/windows/history-window.cpp

	model/buddy-status-dates-model.cpp
	model/chat-dates-model.cpp
	model/history-chats-model.cpp
	model/history-chats-model-proxy.cpp
	model/sms-dates-model.cpp

	search/history-search-parameters.cpp
	storage/history-storage.cpp

	buddy-history-delete-handler.cpp
	history.cpp
	history-messages-prepender.cpp
	history-plugin.cpp
	history-tree-item.cpp
	history-save-thread.cpp
	timed-status.cpp
)

set (MOC_SOURCES
	actions/show-history-action-description.h

	gui/windows/history-window.h

	model/buddy-status-dates-model.h
	model/chat-dates-model.h
	model/history-chats-model.h
	model/history-chats-model-proxy.h
	model/sms-dates-model.h

	storage/history-storage.h

	history.h
	history-messages-prepender.h
	history-plugin.h
	history-save-thread.h
)

set (TRANSLATION_SOURCES
	translations/history_cs.ts
	translations/history_de.ts
	translations/history_en.ts
	translations/history_pl.ts
)

set (CONFIGURATION_FILES
	configuration/history.ui
)

kadu_plugin (history
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_BUILDDEF HISTORY_BUILD
)
