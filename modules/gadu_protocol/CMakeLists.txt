set (SOURCES
	gadu.cpp
	gadu-account-data-manager.cpp
	gadu_account_data.cpp
	gadu_configuration_dialog.cpp
	gadu-contact-account-data.cpp
	gadu_formatter.cpp
	gadu_images_manager.cpp
	gadu-importer.cpp
	gadu_protocol_factory.cpp
	gadu-protocol-socket-notifiers.cpp
	gadu-pubdir-socket-notifiers.cpp
	gadu_search.cpp
	gadu-socket-notifiers.cpp
	gadu-token-socket-notifiers.cpp
	uins_list.cpp
)

set (MOC_SOURCES
	gadu.h
	gadu_configuration_dialog.h
	gadu-contact-account-data.h
	gadu-importer.h
	gadu-protocol-socket-notifiers.h
	gadu-pubdir-socket-notifiers.h
	gadu-socket-notifiers.h
	gadu-token-socket-notifiers.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
	configuration/gadu_protocol.ui
)

kadu_module (gadu_protocol
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
#	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
