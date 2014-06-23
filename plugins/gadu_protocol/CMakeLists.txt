project (gadu_protocol)

cmake_minimum_required (VERSION 2.8)

# Needed for FindQCA2.cmake when building externally
list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules")

set (QT_USE_QTXML 1)
set (QT_USE_QTXMLPATTERNS 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)

find_package (Kadu REQUIRED CONFIG)

set (LIBGADU_MIN_VERSION "1.12.0")

kadu_api_directories (plugins/gadu_protocol
	dcc
	file-transfers
	gui/widgets
	gui/windows
	gui
	helpers
	oauth
	open-chat-with
	server
	services/multilogon
	services
	socket-notifiers
	.
)

set (SOURCES
	dcc/dcc-socket-notifiers.cpp

	file-transfer/gadu-file-transfer-handler.cpp

	gui/widgets/gadu-add-account-widget.cpp
	gui/widgets/gadu-contact-personal-info-widget.cpp
	gui/widgets/gadu-edit-account-widget.cpp
	gui/widgets/gadu-personal-info-widget.cpp

	helpers/formatted-string-gadu-html-visitor.cpp
	helpers/formatted-string-image-key-received-visitor.cpp
	helpers/gadu-imported-contact-xml-receiver.cpp
	helpers/gadu-importer.cpp
	helpers/gadu-list-helper.cpp
	helpers/gadu-protocol-helper.cpp
	helpers/gadu-proxy-helper.cpp

	oauth/oauth-authorization.cpp
	oauth/oauth-authorization-chain.cpp
	oauth/oauth-consumer.cpp
	oauth/oauth-manager.cpp
	oauth/oauth-parameters.cpp
	oauth/oauth-token.cpp
	oauth/oauth-token-fetcher.cpp

	open-chat-with/gadu-open-chat-with-runner.cpp

	server/gadu-avatar-downloader.cpp
	server/gadu-avatar-uploader.cpp
	server/gadu-servers-manager.cpp
	server/gadu-writable-session-token.cpp
	server/protocol-gadu-connection.cpp

	services/gadu-avatar-service.cpp
	services/gadu-chat-image-service.cpp
	services/gadu-chat-service.cpp
	services/gadu-chat-state-service.cpp
	services/gadu-contact-list-service.cpp
	services/gadu-contact-list-state-machine.cpp
	services/gadu-contact-personal-info-service.cpp
	services/gadu-file-transfer-service.cpp
	services/gadu-multilogon-service.cpp
	services/gadu-personal-info-service.cpp
	services/gadu-roster-service.cpp
	services/gadu-search-service.cpp

	services/multilogon/gadu-multilogon-session.cpp

	socket-notifiers/gadu-protocol-socket-notifiers.cpp
	socket-notifiers/gadu-pubdir-socket-notifiers.cpp
	socket-notifiers/gadu-socket-notifiers.cpp

	gadu-account-details.cpp
	gadu-contact-details.cpp
	gadu-id-validator.cpp
	gadu-protocol.cpp
	gadu-protocol-factory.cpp
	gadu-protocol-plugin.cpp
	gadu-status-adapter.cpp
	gadu-url-dom-visitor-provider.cpp
	gadu-url-handler.cpp
)

set (MOC_SOURCES
	dcc/dcc-socket-notifiers.h

	file-transfer/gadu-file-transfer-handler.h

	gui/widgets/gadu-add-account-widget.h
	gui/widgets/gadu-contact-personal-info-widget.h
	gui/widgets/gadu-edit-account-widget.h
	gui/widgets/gadu-personal-info-widget.h

	helpers/formatted-string-image-key-received-visitor.h
	helpers/gadu-importer.h

	oauth/oauth-authorization.h
	oauth/oauth-authorization-chain.h
	oauth/oauth-manager.h
	oauth/oauth-token-fetcher.h

	server/gadu-avatar-downloader.h
	server/gadu-avatar-uploader.h
	server/gadu-connection.h
	server/protocol-gadu-connection.h

	services/gadu-avatar-service.h
	services/gadu-chat-image-service.h
	services/gadu-chat-service.h
	services/gadu-chat-state-service.h
	services/gadu-contact-list-service.h
	services/gadu-contact-list-state-machine.h
	services/gadu-contact-personal-info-service.h
	services/gadu-file-transfer-service.h
	services/gadu-multilogon-service.h
	services/gadu-personal-info-service.h
	services/gadu-roster-service.h
	services/gadu-search-service.h

	services/multilogon/gadu-multilogon-session.h

	socket-notifiers/gadu-protocol-socket-notifiers.h
	socket-notifiers/gadu-pubdir-socket-notifiers.h
	socket-notifiers/gadu-socket-notifiers.h

	gadu-id-validator.h
	gadu-protocol-factory.h
	gadu-protocol-plugin.h
	gadu-url-handler.h
)

set (CONFIGURATION_FILES
)

set (SERVERS_FILE
	data/servers.txt
)

find_package (PkgConfig)
pkg_search_module (LIBGADU REQUIRED libgadu>=${LIBGADU_MIN_VERSION})

find_package (QCA2 REQUIRED)

if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIR ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARIES "-framework qca")
		link_directories (${QCA_FRAMEWORK_DIR}/qca.framework/)
	endif ()
endif ()

add_definitions (${QCA2_DEFINITIONS})
include_directories (${LIBGADU_INCLUDE_DIRS} ${QCA2_INCLUDE_DIR})
link_directories (${LIBGADU_LIBRARY_DIRS})

if (LIBGADU_FOUND)
	message (STATUS " * libgadu (version ${LIBGADU_VERSION}) was found in ${LIBGADU_LIBDIR}")
else ()
	message (FATAL_ERROR " * libgadu (version ${LIBGADU_MIN_VERSION}) was NOT FOUND")
endif ()

set (LIBRARIES ${LIBGADU_LIBRARIES} ${QCA2_LIBRARIES} ${QT_LIBRARIES} ${QT_QTXMLPATTERNS_LIBRARY})
if (WIN32)
	list (APPEND LIBRARIES ws2_32)
endif ()

kadu_plugin (gadu_protocol
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_DATA_FILES ${SERVERS_FILE}
)
