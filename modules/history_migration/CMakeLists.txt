set (SOURCES
	history-import-thread.cpp
	history-migration.cpp
	history-migration-helper.cpp
)

set (MOC_SOURCES
	history-import-thread.h
	history-migration.h
)

set (CONFIGURATION_FILES
	configuration/history-migration.ui
)

#set (TRANSLATION_SOURCES
#	translations/history_migration_pl.ts
#)

kadu_module (history_migration
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	#MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	#TODO: make it more general, eg. gadu_protocol,history_storage
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES gadu_protocol sql_history
)

if (MINGW)
	target_link_libraries (history_migration -L${MINGW_LIBGADU_DIR} -L${MINGW_QCA_DIR}/bin)
endif (MINGW)
