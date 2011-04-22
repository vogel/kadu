project (chat_notifier)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)
message (STATUS "Found Kadu: " ${KADU_INCLUDE_DIR})

include_directories ("${KADU_INCLUDE_DIR}/core")

set (SOURCES
	chat-notifier-plugin.cpp
)

set (MOC_SOURCES
	chat-notifier-plugin.h
)

set (CONFIGURATION_FILES
)

set (TRANSLATION_SOURCES
	translations/chat_notifier_en.ts
	translations/chat_notifier_de.ts
	translations/chat_notifier_fr.ts
	translations/chat_notifier_it.ts
	translations/chat_notifier_pl.ts
)

kadu_plugin (chat_notifier
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
