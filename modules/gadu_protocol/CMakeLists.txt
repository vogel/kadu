set (SOURCES
	dcc/dcc-manager.cpp
	dcc/dcc-socket-notifiers.cpp

	file-transfer/gadu-file-transfer.cpp

	helpers/gadu-formatter.cpp
	helpers/gadu-importer.cpp
	helpers/gadu-list-helper.cpp

	server/gadu-server-connector.cpp
	server/gadu-server-change-password.cpp
	server/gadu-server-register-account.cpp
	server/gadu-server-remind-password.cpp
	server/gadu-server-unregister-account.cpp
	server/gadu-servers-manager.cpp

	services/gadu-chat-image-service.cpp
	services/gadu-chat-service.cpp
	services/gadu-contact-list-service.cpp
	services/gadu-file-transfer-service.cpp
	services/gadu-personal-info-service.cpp
	services/gadu-search-service.cpp

	socket-notifiers/gadu-protocol-socket-notifiers.cpp
	socket-notifiers/gadu-pubdir-socket-notifiers.cpp
	socket-notifiers/gadu-socket-notifiers.cpp
	socket-notifiers/gadu-token-socket-notifiers.cpp

	connection-timeout-timer.cpp
	gadu.cpp
	gadu-account-data-manager.cpp
	gadu_account_data.cpp
	gadu_configuration_dialog.cpp
	gadu-contact-account-data.cpp
	gadu-contact-account-data-manager.cpp
	gadu-contact-account-data-widget.cpp
	gadu_images_manager.cpp
	gadu-protocol.cpp
	gadu_protocol_factory.cpp
	uins_list.cpp
)

set (MOC_SOURCES
	dcc/dcc-manager.h
	dcc/dcc-socket-notifiers.h

	helpers/gadu-importer.h

	file-transfer/gadu-file-transfer.h

	server/gadu-server-connector.h
	server/gadu-server-change-password.h
	server/gadu-server-register-account.h
	server/gadu-server-remind-password.h
	server/gadu-server-unregister-account.h

	services/gadu-chat-image-service.h
	services/gadu-chat-service.h
	services/gadu-contact-list-service.h
	services/gadu-file-transfer-service.h
	services/gadu-personal-info-service.h
	services/gadu-search-service.h

	socket-notifiers/gadu-protocol-socket-notifiers.h
	socket-notifiers/gadu-pubdir-socket-notifiers.h
	socket-notifiers/gadu-socket-notifiers.h
	socket-notifiers/gadu-token-socket-notifiers.h

	connection-timeout-timer.h
	gadu.h
	gadu_configuration_dialog.h
	gadu-contact-account-data.h
	gadu-contact-account-data-widget.h
	gadu-protocol.h
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
