set (SOURCES
	helpers/gadu-list-helper.cpp

	services/gadu-chat-image-service.cpp
	services/gadu-chat-service.cpp
	services/gadu-contact-list-service.cpp

	connection-timeout-timer.cpp
	gadu.cpp
	gadu-account-data-manager.cpp
	gadu_account_data.cpp
	gadu_configuration_dialog.cpp
	gadu-contact-account-data.cpp
	gadu-contact-account-data-manager.cpp
	gadu-contact-account-data-widget.cpp
	gadu_formatter.cpp
	gadu_images_manager.cpp
	gadu-importer.cpp
	gadu-protocol.cpp
	gadu_protocol_factory.cpp
	gadu-protocol-socket-notifiers.cpp
	gadu-pubdir-socket-notifiers.cpp
	gadu-search.cpp
	gadu-server-connector.cpp
	gadu-server-change-password.cpp
	gadu-server-register-account.cpp
	gadu-server-remind-password.cpp
	gadu-server-unregister-account.cpp
	gadu-socket-notifiers.cpp
	gadu-token-socket-notifiers.cpp
	uins_list.cpp
)

set (MOC_SOURCES
	services/gadu-chat-image-service.h
	services/gadu-chat-service.h
	services/gadu-contact-list-service.h

	connection-timeout-timer.h
	gadu.h
	gadu_configuration_dialog.h
	gadu-contact-account-data.h
	gadu-contact-account-data-widget.h
	gadu-importer.h
	gadu-protocol.h
	gadu-protocol-socket-notifiers.h
	gadu-pubdir-socket-notifiers.h
	gadu-search.h
	gadu-server-connector.h
	gadu-server-change-password.h
	gadu-server-register-account.h
	gadu-server-remind-password.h
	gadu-server-unregister-account.h
	gadu-socket-notifiers.h
	gadu-token-socket-notifiers.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
	configuration/gadu_protocol.ui
	configuration/gadu_contact.ui
)

kadu_module (gadu_protocol
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
#	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
