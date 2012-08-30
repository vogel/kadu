project (exec_notify)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	exec_notify.cpp
	exec-notify-plugin.cpp
)

set (MOC_SOURCES
	exec_notify.h
	exec-notify-plugin.h
)

kadu_plugin (exec_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
