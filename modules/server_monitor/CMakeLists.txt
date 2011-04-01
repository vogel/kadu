set (SOURCES
	server-monitor.cpp
	server-status-widget.cpp
	server-monitor-window.cpp
)

set (MOC_SOURCES
	server-monitor.h
	server-status-widget.h
	server-monitor-window.h
)

set (TRANSLATION_SOURCES
	translations/server_monitor_en.ts
	translations/server_monitor_de.ts
	translations/server_monitor_fr.ts
	translations/server_monitor_it.ts
	translations/server_monitor_pl.ts
)

set (CONFIGURATION_FILES
	configuration/server-monitor.ui
	configuration/serverslist.txt
)

kadu_plugin (server_monitor
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES gadu_protocol
)

if (MINGW)
	target_link_libraries (server_monitor -L${MINGW_LIBGADU_DIR} -L${MINGW_QCA_DIR}/bin)
endif (MINGW)
