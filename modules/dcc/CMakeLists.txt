set (SOURCES
	dcc.cpp
	dcc_handler.cpp
	dcc_socket.cpp
	file_transfer.cpp
	file_transfer_manager.cpp
	file_transfer_notifications.cpp
	file_transfer_window.cpp
)

set (MOC_SOURCES
	dcc.h
	dcc_handler.h
	dcc_socket.h
	file_transfer.h
	file_transfer_manager.h
	file_transfer_notifications.h
	file_transfer_window.h
)

set (TRANSLATION_SOURCES
	translations/dcc_de.ts
	translations/dcc_fr.ts
	translations/dcc_it.ts
	translations/dcc_pl.ts
)

set (CONFIGURATION_FILES
	configuration/dcc.ui)

kadu_module (dcc
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
