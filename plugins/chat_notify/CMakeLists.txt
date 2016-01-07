project (chat_notify)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	chat-notifier.cpp
	chat-notify-module.cpp
	chat-notify-plugin-modules-factory.cpp
	chat-notify-plugin-object.cpp
)

set (CONFIGURATION_FILES
)

kadu_plugin (chat_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
