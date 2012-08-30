project (gadu_protocol)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (QT_USE_QTXML 1)
set (QT_USE_QTXMLPATTERNS 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

include (FindPkgConfig)

set (LIBGADU_MIN_VERSION "1.11.1")

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
	gui/widgets/gadu-create-account-widget.cpp
	gui/widgets/gadu-contact-personal-info-widget.cpp
	gui/widgets/gadu-edit-account-widget.cpp
	gui/widgets/gadu-personal-info-widget.cpp
	gui/widgets/token-widget.cpp
	gui/windows/gadu-change-password-window.cpp
	gui/windows/gadu-remind-password-window.cpp
	gui/windows/gadu-unregister-account-window.cpp
	gui/windows/gadu-wait-for-account-register-window.cpp

	helpers/gadu-formatter.cpp
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

	server/gadu-avatar-data-parser.cpp
	server/gadu-avatar-fetcher.cpp
	server/gadu-avatar-uploader.cpp
	server/gadu-server-change-password.cpp
	server/gadu-server-register-account.cpp
	server/gadu-server-remind-password.cpp
	server/gadu-server-unregister-account.cpp
	server/gadu-servers-manager.cpp
	server/gadu-token-fetcher.cpp

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
	gadu-url-handler.cpp
)

set (MOC_SOURCES
	dcc/dcc-socket-notifiers.h

	file-transfer/gadu-file-transfer-handler.h

	gui/widgets/gadu-add-account-widget.h
	gui/widgets/gadu-contact-personal-info-widget.h
	gui/widgets/gadu-create-account-widget.h
	gui/widgets/gadu-edit-account-widget.h
	gui/widgets/gadu-personal-info-widget.h
	gui/widgets/token-widget.h
	gui/windows/gadu-change-password-window.h
	gui/windows/gadu-remind-password-window.h
	gui/windows/gadu-unregister-account-window.h
	gui/windows/gadu-wait-for-account-register-window.h

	helpers/gadu-importer.h

	oauth/oauth-authorization.h
	oauth/oauth-authorization-chain.h
	oauth/oauth-manager.h
	oauth/oauth-token-fetcher.h

	server/gadu-avatar-fetcher.h
	server/gadu-avatar-uploader.h
	server/gadu-server-change-password.h
	server/gadu-server-register-account.h
	server/gadu-server-remind-password.h
	server/gadu-server-unregister-account.h
	server/gadu-token-fetcher.h

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
	gadu-protocol.h
	gadu-url-handler.h
)

set (CONFIGURATION_FILES
)

set (SERVERS_FILE
	data/servers.txt
)

if (WIN32)
	if (NOT WIN_LIBGADU_DIR)
		message (SEND_ERROR "Pass path to libgadu -DWIN_LIBGADU_DIR=path")
		set (WIN_LIBGADU_DIR "")
	else (NOT WIN_LIBGADU_DIR)
		set (LIBGADU_FOUND TRUE)
		set (LIBGADU_INCLUDE_DIRS ${WIN_LIBGADU_DIR}/include)
		set (LIBGADU_LIBRARIES optimized gadu debug gadud)

		if (MSVC)
			set (LIBGADU_LIBRARY_DIRS ${WIN_LIBGADU_DIR}/lib)
		else (MSVC)
			set (LIBGADU_LIBRARY_DIRS ${WIN_LIBGADU_DIR}/bin)
		endif (MSVC)
	endif (NOT WIN_LIBGADU_DIR)
	if (NOT WIN_QCA_DIR)
		message (SEND_ERROR "Pass path to libqca2 -DWIN_QCA_DIR=path")
	else (NOT WIN_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${WIN_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARIES optimized qca2 debug qcad2)

		if (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/lib)
		else (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/bin)
		endif (MSVC)
	endif (NOT WIN_QCA_DIR)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2)
	pkg_search_module (LIBGADU REQUIRED libgadu>=${LIBGADU_MIN_VERSION})
endif (WIN32)

if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARY_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/)
		set (QCA2_LIBRARIES "-framework qca")
	endif (QCA_FRAMEWORK_DIR)
endif (APPLE)

include_directories (${LIBGADU_INCLUDE_DIRS} ${QCA2_INCLUDE_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
link_directories (${QCA2_LIBRARY_DIRS})
link_directories (${LIBGADU_LIBRARY_DIRS})

if (LIBGADU_FOUND)
	message (STATUS " * libgadu (version ${LIBGADU_VERSION}) was found in ${LIBGADU_LIBDIR}")
else (LIBGADU_FOUND)
	message (FATAL_ERROR " * libgadu (version ${LIBGADU_MIN_VERSION}) was NOT FOUND")
endif (LIBGADU_FOUND)

set (LIBRARIES ${LIBGADU_LIBRARIES} ${QCA2_LIBRARIES} ${QT_LIBRARIES} ${QT_QTXMLPATTERNS_LIBRARY})
if (WIN32)
	list (APPEND LIBRARIES ws2_32)
endif (WIN32)

kadu_plugin (gadu_protocol
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_DATA_FILES ${SERVERS_FILE}
	PLUGIN_BUILDDEF GADU_BUILD
)
