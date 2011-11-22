project (sql_history)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	storage/history-sql-storage.cpp
	storage/sql-initializer.cpp
	sql-history-plugin.cpp
)

set (MOC_SOURCES
	storage/history-sql-storage.h
	storage/sql-initializer.h
	sql-history-plugin.h
)

set (TRANSLATION_SOURCES
	translations/sql_history_cs.ts
	translations/sql_history_de.ts
	translations/sql_history_en.ts
	translations/sql_history_pl.ts
)

set (CONFIGURATION_FILES
)

kadu_plugin (sql_history
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QT_QTSQL_LIBRARIES}
	PLUGIN_DEPENDENCIES history
)
