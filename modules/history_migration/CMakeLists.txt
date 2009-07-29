set (SOURCES
	history-migration.cpp
)

#set (TRANSLATION_SOURCES
#	translations/history_migration_pl.ts
#)

kadu_module (history_migration
	MODULE_SOURCES ${SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	#TODO: make it more general, eg. gadu_protocol,history_storage
	MODULE_DEPENDENCIES gadu_protocol sql_history
)
