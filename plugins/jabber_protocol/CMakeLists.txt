project (jabber_protocol)

cmake_minimum_required (VERSION 2.8)

# Needed for FindQCA2.cmake when building externally
list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules")

find_package (Kadu REQUIRED CONFIG)

find_package (PkgConfig)
pkg_search_module (IDN REQUIRED libidn)
pkg_search_module (ZLIB zlib)

if (NOT ZLIB_FOUND)
	find_package (ZLIB REQUIRED)
endif ()

find_package (QCA2 REQUIRED)

if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIR ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARIES "-framework qca")
		link_directories (${QCA_FRAMEWORK_DIR}/qca.framework/)
	endif ()
endif ()

kadu_api_directories (plugins/jabber_protocol
	actions
	certificates
	client
	file-transfer
	gui/widgets
	gui/windows
	gui
	open-chat-with
	resource
	server
	services
	tasks
	utils
	.
)

set (SOURCES
	actions/jabber-actions.cpp
	actions/jabber-protocol-menu-manager.cpp
	actions/show-xml-console-action-description.cpp

	certificates/certificate-display-dialog.cpp
	certificates/certificate-helpers.cpp
	certificates/trusted-certificates-manager.cpp

	client/mini-client.cpp

	file-transfer/jabber-file-transfer-handler.cpp
	file-transfer/s5b-server-manager.cpp

	gui/widgets/jabber-contact-personal-info-widget.cpp
	gui/widgets/jabber-add-account-widget.cpp
	gui/widgets/jabber-create-account-widget.cpp
	gui/widgets/jabber-edit-account-widget.cpp
	gui/widgets/jabber-personal-info-widget.cpp

	gui/windows/certificate-error-window.cpp
	gui/windows/jabber-change-password-window.cpp
	gui/windows/jabber-wait-for-account-register-window.cpp
	gui/windows/xml-console.cpp

	open-chat-with/jabber-open-chat-with-runner.cpp

	resource/jabber-resource.cpp
	resource/jabber-resource-pool.cpp

	server/jabber-avatar-downloader.cpp
	server/jabber-avatar-pep-downloader.cpp
	server/jabber-avatar-pep-uploader.cpp
	server/jabber-avatar-uploader.cpp
	server/jabber-avatar-vcard-downloader.cpp
	server/jabber-avatar-vcard-uploader.cpp
	server/jabber-server-change-password.cpp
	server/jabber-server-register-account.cpp

	services/jabber-avatar-service.cpp
	services/jabber-chat-service.cpp
	services/jabber-chat-state-service.cpp
	services/jabber-client-info-service.cpp
	services/jabber-connection-service.cpp
	services/jabber-contact-personal-info-service.cpp
	services/jabber-file-transfer-service.cpp
	services/jabber-pep-service.cpp
	services/jabber-personal-info-service.cpp
	services/jabber-roster-service.cpp
	services/jabber-server-info-service.cpp
	services/jabber-stream-debug-service.cpp
	services/jabber-subscription-service.cpp
	services/jabber-vcard-downloader.cpp
	services/jabber-vcard-service.cpp
	services/jabber-vcard-uploader.cpp

	tasks/pep-get-task.cpp
	tasks/pep-publish-task.cpp
	tasks/pep-retract-task.cpp

	iris-status-adapter.cpp
	jabber-account-details.cpp
	jabber-contact-details.cpp
	jabber-error-helper.cpp
	jabber-id-validator.cpp
	jabber-protocol.cpp
	jabber-protocol-factory.cpp
	jabber-protocol-plugin.cpp
	jabber-status-adapter.cpp
	jabber-url-dom-visitor-provider.cpp
	jabber-url-handler.cpp
	facebook-protocol-factory.cpp
	gtalk-protocol-factory.cpp
)

set (MOC_SOURCES
	actions/jabber-actions.h
	actions/show-xml-console-action-description.h

	certificates/certificate-display-dialog.h
	certificates/trusted-certificates-manager.h

	client/mini-client.h

	file-transfer/jabber-file-transfer-handler.h
	file-transfer/s5b-server-manager.h

	gui/widgets/jabber-contact-personal-info-widget.h
	gui/widgets/jabber-add-account-widget.h
	gui/widgets/jabber-create-account-widget.h
	gui/widgets/jabber-edit-account-widget.h
	gui/widgets/jabber-personal-info-widget.h

	gui/windows/certificate-error-window.h
	gui/windows/jabber-change-password-window.h
	gui/windows/jabber-wait-for-account-register-window.h
	gui/windows/xml-console.h

	resource/jabber-resource.h
	resource/jabber-resource-pool.h

	server/jabber-avatar-downloader.h
	server/jabber-avatar-pep-downloader.h
	server/jabber-avatar-pep-uploader.h
	server/jabber-avatar-uploader.h
	server/jabber-avatar-vcard-downloader.h
	server/jabber-avatar-vcard-uploader.h
	server/jabber-server-change-password.h
	server/jabber-server-register-account.h

	services/jabber-avatar-service.h
	services/jabber-chat-service.h
	services/jabber-chat-state-service.h
	services/jabber-client-info-service.h
	services/jabber-connection-service.h
	services/jabber-contact-personal-info-service.h
	services/jabber-file-transfer-service.h
	services/jabber-pep-service.h
	services/jabber-personal-info-service.h
	services/jabber-roster-service.h
	services/jabber-server-info-service.h
	services/jabber-stream-debug-service.h
	services/jabber-subscription-service.h
	services/jabber-vcard-downloader.h
	services/jabber-vcard-service.h
	services/jabber-vcard-uploader.h

	tasks/pep-get-task.h
	tasks/pep-publish-task.h
	tasks/pep-retract-task.h

	facebook-protocol-factory.h
	gtalk-protocol-factory.h
	jabber-id-validator.h
	jabber-protocol.h
	jabber-protocol-factory.h
	jabber-protocol-plugin.h
	jabber-url-handler.h
)

add_definitions (${QCA2_DEFINITIONS} ${IDN_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER} -DIRISNET_STATIC)
add_subdirectory (3rdparty)

if (WIN32)
	include_directories (
		${QCA2_INCLUDE_DIR}

		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/include
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/include/iris
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/src/irisnet/corelib
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/src/xmpp/base64/
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/src/xmpp/jid/
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/src/xmpp/xmpp-core/
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris-win/src
	)
else ()
	include_directories (
		${QCA2_INCLUDE_DIR}

		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/include
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/include/iris
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/irisnet/corelib
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/xmpp/base64/
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/xmpp/jid/
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/xmpp/xmpp-core/
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src
	)
endif ()

set (LIBRARIES 3rdparty ${QCA2_LIBRARIES} ${IDN_LIBRARIES} ${ZLIB_LIBRARIES} ${QT_LIBRARIES})
if (WIN32)
	set (LIBRARIES ${LIBRARIES} advapi32 ws2_32)
endif ()

link_directories (${CMAKE_CURRENT_BUILD_DIR}/3rdparty ${IDN_LIBRARY_DIRS} ${ZLIB_LIBRARY_DIRS})

set (CONFIGURATION_FILES
	data/configuration/jabber_protocol.ui
)

kadu_plugin (jabber_protocol
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

add_dependencies (jabber_protocol
	3rdparty
)
