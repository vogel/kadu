project (jabber_protocol)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

find_package (PkgConfig)
pkg_search_module (IDN REQUIRED libidn)
pkg_search_module (ZLIB REQUIRED zlib)
pkg_search_module (QXMPP REQUIRED qxmpp>=0.8.3)

if (NOT ZLIB_FOUND)
	find_package (ZLIB REQUIRED)
endif ()

kadu_api_directories (plugins/jabber_protocol
	actions
	file-transfer
	gui/widgets
	gui/windows
	gui
	open-chat-with
	qxmpp
	services
	.
)

set (SOURCES
	actions/ask-for-subscription-action.cpp
	actions/jabber-actions.cpp
	actions/jabber-protocol-menu-manager.cpp
	actions/remove-subscription-action.cpp
	actions/resend-subscription-action.cpp
	actions/show-xml-console-action.cpp
	actions/subscription-action.cpp

	file-transfer/jabber-outgoing-file-transfer-handler.cpp
	file-transfer/jabber-stream-incoming-file-transfer-handler.cpp

	gui/widgets/jabber-contact-personal-info-widget.cpp
	gui/widgets/jabber-add-account-widget.cpp
	gui/widgets/jabber-create-account-widget.cpp
	gui/widgets/jabber-edit-account-widget.cpp
	gui/widgets/jabber-personal-info-widget.cpp

	gui/windows/jabber-change-password-window.cpp
	gui/windows/jabber-wait-for-account-register-window.cpp
	gui/windows/xml-console.cpp

	open-chat-with/jabber-open-chat-with-runner.cpp

	qxmpp/jabber-register-extension.cpp
	qxmpp/jabber-roster-extension.cpp
	qxmpp/jabber-ssl-handler.cpp

	services/jabber-avatar-downloader.cpp
	services/jabber-avatar-service.cpp
	services/jabber-avatar-uploader.cpp
	services/jabber-change-password-service.cpp
	services/jabber-change-password.cpp
	services/jabber-chat-service.cpp
	services/jabber-chat-state-service.cpp
	services/jabber-contact-personal-info-service.cpp
	services/jabber-error-service.cpp
	services/jabber-file-transfer-service.cpp
	services/jabber-personal-info-service.cpp
	services/jabber-presence-service.cpp
	services/jabber-register-account-service.cpp
	services/jabber-register-account.cpp
	services/jabber-resource.cpp
	services/jabber-resource-service.cpp
	services/jabber-room-chat-service.cpp
	services/jabber-room-chat.cpp
	services/jabber-roster-service.cpp
	services/jabber-servers-service.cpp
	services/jabber-stream-debug-service.cpp
	services/jabber-subscription-service.cpp
	services/jabber-vcard-downloader.cpp
	services/jabber-vcard-service.cpp
	services/jabber-vcard-uploader.cpp

	facebook-depreceated-message.cpp
	jabber-account-data.cpp
	jabber-id-validator.cpp
	jabber-module.cpp
	jabber-plugin-modules-factory.cpp
	jabber-plugin-object.cpp
	jabber-protocol.cpp
	jabber-protocol-factory.cpp
	jabber-url-dom-visitor-provider.cpp
	jabber-url-handler.cpp
	jid.cpp
	gtalk-protocol-factory.cpp
)

add_definitions (${IDN_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER} ${QXMPP_DEFINITIONS} ${QXMPP_CFLAGS_OTHER})

set (LIBRARIES ${LIBRARIES} ${IDN_LIBRARIES} ${ZLIB_LIBRARIES} ${QXMPP_LIBRARIES})
if (WIN32)
	set (LIBRARIES ${LIBRARIES} advapi32 ws2_32)
endif ()

include_directories (${QXMPP_INCLUDE_DIRS})
link_directories (${IDN_LIBRARY_DIRS} ${ZLIB_LIBRARY_DIRS} ${QXMPP_LIBRARY_DIRS})

kadu_plugin (jabber_protocol
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
