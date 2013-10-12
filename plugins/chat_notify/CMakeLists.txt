project (chat_notify)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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

kadu_plugin (chat_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
