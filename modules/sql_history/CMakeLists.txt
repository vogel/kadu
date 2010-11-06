set (SOURCES
	storage/history-sql-storage.cpp
	sql-history-module.cpp
)

set (MOC_SOURCES
	storage/history-sql-storage.h
)

set (TRANSLATION_SOURCES
	translations/sql_history_pl.ts
)

set (CONFIGURATION_FILES
)

kadu_module (sql_history
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES history
)

target_link_libraries (sql_history ${QT_QTSQL_LIBRARIES})
