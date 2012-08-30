project (sql_history)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

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

	sql-history-plugin.cpp
)

set (MOC_SOURCES
	storage/history-sql-storage.h
	storage/sql-accounts-mapping.h
	storage/sql-chats-mapping.h
	storage/sql-contacts-mapping.h
	storage/sql-import.h
	storage/sql-initializer.h
	storage/sql-messages-chat-storage.h
	storage/sql-messages-sms-storage.h
	storage/sql-messages-status-storage.h
	storage/sql-restore.h

	sql-history-plugin.h
)

set (CONFIGURATION_FILES
)

kadu_plugin (sql_history
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QT_QTSQL_LIBRARIES}
	PLUGIN_DEPENDENCIES history
)

if (NOT WIN32)
	install (PROGRAMS data/scripts/history-database-recovery.sh
		DESTINATION ${KADU_PLUGINS_DIR}/data/sql_history/scripts
	)
endif (NOT WIN32)
