project (chat_notify)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	chat-notifier.cpp
	chat-notify-plugin.cpp
)

set (MOC_SOURCES
	chat-notifier.h
	chat-notify-plugin.h
)

set (CONFIGURATION_FILES
)

set (TRANSLATION_SOURCES
	translations/chat_notify_en.ts
	translations/chat_notify_de.ts
	translations/chat_notify_pl.ts
)

kadu_plugin (chat_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
