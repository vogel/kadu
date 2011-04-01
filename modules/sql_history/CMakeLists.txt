set (SOURCES
	storage/history-sql-storage.cpp
	sql-history-module.cpp
)

set (MOC_SOURCES
	storage/history-sql-storage.h
)

set (TRANSLATION_SOURCES
	translations/sql_history_en.ts
	translations/sql_history_de.ts
	translations/sql_history_fr.ts
	translations/sql_history_it.ts
	translations/sql_history_pl.ts
)

set (CONFIGURATION_FILES
)

kadu_plugin (sql_history
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES history
)

target_link_libraries (sql_history ${QT_QTSQL_LIBRARIES})
