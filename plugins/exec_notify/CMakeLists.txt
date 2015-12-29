project (exec_notify)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	exec-configuration-widget.cpp
	exec-notifier.cpp
	exec-notify-module.cpp
	exec-notify-plugin-injector-factory.cpp
	exec-notify-plugin-object.cpp
)

kadu_plugin (exec_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
