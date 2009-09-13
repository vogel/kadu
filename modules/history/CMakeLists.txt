set (SOURCES
# 	gui/windows/history-search-dialog.cpp
	gui/windows/history-window.cpp
	model/chat-dates-model.cpp
	model/history-chats-model.cpp
	model/history-chats-model-proxy.cpp
	search/history-search-parameters.cpp
	storage/history-storage.cpp
	history.cpp
)

set (MOC_SOURCES
# 	gui/windows/history-search-dialog.h
	gui/windows/history-window.h
	model/chat-dates-model.h
	model/history-chats-model.h
	model/history-chats-model-proxy.h
	storage/history-storage.h
	history.h
)

set (TRANSLATION_SOURCES
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
