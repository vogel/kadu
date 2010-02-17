set (SOURCES

	dcc/dcc-manager.cpp
	dcc/dcc-socket-notifiers.cpp

	file-transfer/gadu-file-transfer-handler.cpp

	gui/widgets/gadu-add-account-widget.cpp
	gui/widgets/gadu-create-account-widget.cpp
	gui/widgets/gadu-edit-account-widget.cpp
	gui/widgets/gadu-personal-info-widget.cpp
	gui/widgets/token-widget.cpp
	gui/windows/gadu-change-password-window.cpp
	gui/windows/gadu-remind-password-window.cpp
	gui/windows/gadu-unregister-account-window.cpp

	helpers/gadu-formatter.cpp
	helpers/gadu-importer.cpp
	helpers/gadu-list-helper.cpp

	oauth/oauth-authorization.cpp
	oauth/oauth-authorization-chain.cpp
	oauth/oauth-consumer.cpp
	oauth/oauth-manager.cpp
	oauth/oauth-parameters.cpp
	oauth/oauth-token.cpp
	oauth/oauth-token-fetcher.cpp

	open-chat-with/gadu-open-chat-with-runner.cpp

	server/gadu-avatar-fetcher.cpp
	server/gadu-avatar-uploader.cpp
	server/gadu-server-connector.cpp
	server/gadu-server-change-password.cpp
	server/gadu-server-register-account.cpp
	server/gadu-server-remind-password.cpp
	server/gadu-server-unregister-account.cpp
	server/gadu-servers-manager.cpp
	server/token-fetcher.cpp

	services/gadu-avatar-service.cpp
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

	gadu-account-data-manager.cpp
	gadu-account-details.cpp
	gadu-contact-data-manager.cpp
	gadu-contact-details.cpp
	gadu-contact-widget.cpp
	gadu-protocol.cpp
	gadu-protocol-factory.cpp
	gadu-resolver.cpp
	gadu-url-handler.cpp
)

set (MOC_SOURCES
	dcc/dcc-manager.h
	dcc/dcc-socket-notifiers.h

	file-transfer/gadu-file-transfer-handler.h

	gui/widgets/gadu-add-account-widget.h
	gui/widgets/gadu-create-account-widget.h
	gui/widgets/gadu-edit-account-widget.h
	gui/widgets/gadu-personal-info-widget.h
	gui/widgets/token-widget.h
	gui/windows/gadu-change-password-window.h
	gui/windows/gadu-remind-password-window.h
	gui/windows/gadu-unregister-account-window.h

	helpers/gadu-importer.h

	oauth/oauth-authorization.h
	oauth/oauth-authorization-chain.h
	oauth/oauth-manager.h
	oauth/oauth-token-fetcher.h

	server/gadu-avatar-fetcher.h
	server/gadu-avatar-uploader.h
	server/gadu-server-connector.h
	server/gadu-server-change-password.h
	server/gadu-server-register-account.h
	server/gadu-server-remind-password.h
	server/gadu-server-unregister-account.h
	server/token-fetcher.h

	services/gadu-avatar-service.h
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

	#gadu-account-details.h
	gadu-contact-widget.h
	gadu-protocol.h
	gadu-resolver.h
	gadu-url-handler.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
	configuration/gadu_contact.ui
)

pkg_search_module (QCA2 REQUIRED qca2)
link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})

pkg_search_module (LIBGADU REQUIRED libgadu>=1.9.0)
include_directories (${LIBGADU_INCLUDE_DIRS} ${QCA2_INCLUDE_DIRS})
link_directories (${LIBGADU_LIBRARY_DIRS})

message (STATUS " * libgadu found (version ${LIBGADU_VERSION}) in ${LIBGADU_LIBRARY_DIRS}")

kadu_module (gadu_protocol
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
#	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${LIBGADU_LIBRARIES} ${QCA2_LIBRARIES}
	MODULE_DEPENDENCIES ${QCA2_LIBRARIES}
)
