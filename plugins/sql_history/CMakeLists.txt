project (sql_history)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	storage/history-sql-storage.cpp
	storage/sql-accounts-mapping.cpp
	storage/sql-chats-mapping.cpp
	storage/sql-contacts-mapping.cpp
	storage/sql-import.cpp
	storage/sql-initializer.cpp
	storage/sql-messages-chat-storage.cpp
	storage/sql-messages-sms-storage.cpp
	storage/sql-messages-status-storage.cpp
	storage/sql-restore.cpp

	sql-history-module.cpp
	sql-history-plugin-injector-factory.cpp
	sql-history-plugin-object.cpp
)

kadu_plugin (sql_history
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_DEPENDENCIES history
	PLUGIN_ADDITIONAL_QT_MODULES Concurrent Sql
)

if (NOT WIN32)
	install (PROGRAMS data/scripts/history-database-recovery.sh
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/sql_history/scripts
	)
endif ()
