set (SOURCES
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
	history-module.cpp
	history-tree-item.cpp
	history-save-thread.cpp
	timed-status.cpp
)

set (MOC_SOURCES
	gui/windows/history-window.h

	model/buddy-status-dates-model.h
	model/chat-dates-model.h
	model/history-chats-model.h
	model/history-chats-model-proxy.h
	model/sms-dates-model.h

	storage/history-storage.h

	history.h
	history-save-thread.h
)

set (TRANSLATION_SOURCES
	translations/history_de.ts
	translations/history_fr.ts
	translations/history_it.ts
	translations/history_pl.ts
)

set (CONFIGURATION_FILES
	configuration/history.ui
)

kadu_module (history
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF HISTORY_BUILD
)
