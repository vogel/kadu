set (SOURCES
	gui/windows/history-import-window.cpp

	history-import-thread.cpp
	history-importer.cpp
	history-importer-chat-data.cpp
	history-importer-manager.cpp
	history-migration-actions.cpp
	history-migration-helper.cpp
	history-migration-module.cpp
)

set (MOC_SOURCES
	gui/windows/history-import-window.h

	history-migration-actions.h
	history-import-thread.h
	history-importer.h
	history-importer-chat-data.h
	history-importer-manager.h
)

set (CONFIGURATION_FILES
)

set (TRANSLATION_SOURCES
	translations/history_migration_de.ts
	translations/history_migration_fr.ts
	translations/history_migration_it.ts
	translations/history_migration_pl.ts
)

kadu_module (history_migration
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	#TODO: make it more general, eg. gadu_protocol,history_storage
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES gadu_protocol sql_history
	MODULE_BUILDDEF HISTORY_MIGRATION_BUILD
)

if (MINGW)
	target_link_libraries (history_migration -L${MINGW_LIBGADU_DIR} -L${MINGW_QCA_DIR}/bin)
endif (MINGW)
