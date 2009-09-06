set (SOURCES
	gui/windows/history-search-dialog.cpp
	gui/windows/history-window.cpp
	models/history-chats-model.cpp
	storage/history-storage.cpp
	history.cpp
)

set (MOC_SOURCES
	gui/windows/history-search-dialog.h
	gui/windows/history-window.h
	models/history-chats-model.h
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
