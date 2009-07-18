set (SOURCES
	history.cpp
	gui/windows/history-dialog.cpp
	gui/windows/history-search-dialog.cpp
	storage/history-storage.cpp
)

set (MOC_SOURCES
	history.h
	gui/windows/history-dialog.h
	gui/windows/history-search-dialog.h
	storage/history-storage.h
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
