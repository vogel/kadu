set (SOURCES                                                                                                                                                               
        server_monitor.cpp
        server_status.cpp
        server_monitor_window.cpp
)

set (MOC_SOURCES
        server_monitor.h
        server_status.h
        server_monitor_window.h
)

set (TRANSLATION_SOURCES
        translations/server_monitor_pl.ts)

set (CONFIGURATION_FILES
        configuration/server_monitor.ui
        configuration/serverslist.txt
)

kadu_module (server_monitor
        MODULE_SOURCES ${SOURCES}
        MODULE_MOC_SOURCES ${MOC_SOURCES}
        MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
        MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
