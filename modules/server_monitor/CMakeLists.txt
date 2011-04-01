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

kadu_module (server_monitor
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES gadu_protocol
)

if (MINGW)
	target_link_libraries (server_monitor -L${MINGW_LIBGADU_DIR} -L${MINGW_QCA_DIR}/bin)
endif (MINGW)
