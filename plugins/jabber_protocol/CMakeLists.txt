project (jabber_protocol)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

include (FindPkgConfig)

if (WIN32)
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

	if (NOT WIN_ZLIB_DIR)
		message (SEND_ERROR "Pass path to zlib -DWIN_ZLIB_DIR=path")
	else (NOT WIN_ZLIB_DIR)
		set (ZLIB_INCLUDE_DIRS ${WIN_ZLIB_DIR}/include)

		if (MSVC)
			set (ZLIB_LIBRARY_DIRS ${WIN_ZLIB_DIR}/lib)
			set (ZLIB_LIBRARIES zlib)
		else (MSVC)
			set (ZLIB_LIBRARY_DIRS ${WIN_ZLIB_DIR}/bin)
			set (ZLIB_LIBRARIES zlib1)
		endif (MSVC)
	endif (NOT WIN_ZLIB_DIR)

	if (NOT WIN_IDN_DIR)
		message (SEND_ERROR "Pass path to libidn -DWIN_IDN_DIR=path")
	else (NOT WIN_IDN_DIR)
		if (MSVC)
			set (IDN_INCLUDE_DIRS ${WIN_IDN_DIR}/win32/include ${WIN_IDN_DIR}/lib)
			set (IDN_LIBRARIES optimized ${WIN_IDN_DIR}/win32/lib/libidn.lib debug ${WIN_IDN_DIR}/win32/lib/debug/libidn.lib)
		else (MSVC)
			set (IDN_INCLUDE_DIRS ${WIN_IDN_DIR}/include)
			set (IDN_LIBRARY_DIRS ${WIN_IDN_DIR}/bin)
			set (IDN_LIBRARIES idn-11)
		endif (MSVC)
	endif (NOT WIN_IDN_DIR)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2)
	pkg_search_module (IDN REQUIRED libidn)

	include (FindZLIB)
	if (NOT ZLIB_FOUND)
		pkg_search_module (ZLIB REQUIRED zlib)
	endif (NOT ZLIB_FOUND)
endif (WIN32)

if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARY_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/)
		set (QCA2_LIBRARIES "-framework qca")
	endif (QCA_FRAMEWORK_DIR)
endif (APPLE)

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

	client/jabber-client.cpp
	client/mini-client.cpp
	client/pong-server.cpp

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

	server/jabber-avatar-fetcher.cpp
	server/jabber-avatar-pep-fetcher.cpp
	server/jabber-avatar-pep-uploader.cpp
	server/jabber-avatar-uploader.cpp
	server/jabber-avatar-vcard-fetcher.cpp
	server/jabber-avatar-vcard-uploader.cpp
	server/jabber-server-change-password.cpp
	server/jabber-server-register-account.cpp

	services/jabber-avatar-service.cpp
	services/jabber-chat-service.cpp
	services/jabber-chat-state-service.cpp
	services/jabber-contact-personal-info-service.cpp
	services/jabber-file-transfer-service.cpp
	services/jabber-personal-info-service.cpp
	services/jabber-roster-service.cpp
	services/jabber-subscription-service.cpp

	tasks/pep-get-task.cpp
	tasks/pep-publish-task.cpp
	tasks/pep-retract-task.cpp

	utils/jid-util.cpp
	utils/pep-manager.cpp
	utils/server-info-manager.cpp
	utils/vcard-factory.cpp

	iris-status-adapter.cpp
	jabber-account-details.cpp
	jabber-contact-details.cpp
	jabber-id-validator.cpp
	jabber-protocol.cpp
	jabber-protocol-factory.cpp
	jabber-protocol-plugin.cpp
	jabber-status-adapter.cpp
	jabber-url-handler.cpp
	facebook-protocol-factory.cpp
	gtalk-protocol-factory.cpp
)

set (MOC_SOURCES
	actions/jabber-actions.h
	actions/show-xml-console-action-description.h

	certificates/certificate-display-dialog.h
	certificates/trusted-certificates-manager.h

	client/jabber-client.h
	client/mini-client.h
	client/pong-server.h

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

	server/jabber-avatar-fetcher.h
	server/jabber-avatar-pep-fetcher.h
	server/jabber-avatar-pep-uploader.h
	server/jabber-avatar-uploader.h
	server/jabber-avatar-vcard-fetcher.h
	server/jabber-avatar-vcard-uploader.h
	server/jabber-server-change-password.h
	server/jabber-server-register-account.h

	services/jabber-avatar-service.h
	services/jabber-chat-service.h
	services/jabber-chat-state-service.h
	services/jabber-contact-personal-info-service.h
	services/jabber-file-transfer-service.h
	services/jabber-personal-info-service.h
	services/jabber-roster-service.h
	services/jabber-subscription-service.h

	tasks/pep-get-task.h
	tasks/pep-publish-task.h
	tasks/pep-retract-task.h

	utils/pep-manager.h
	utils/server-info-manager.h
	utils/vcard-factory.h

	facebook-protocol-factory.h
	gtalk-protocol-factory.h
	jabber-id-validator.h
	jabber-protocol.h
	jabber-protocol-factory.h
	jabber-protocol-plugin.h
	jabber-url-handler.h
)

add_definitions (${QCA2_CFLAGS_OTHER} ${IDN_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER} -DIRISNET_STATIC)
add_subdirectory (3rdparty)

include_directories (
	${QCA2_INCLUDE_DIRS}

	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/include/iris
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/irisnet/corelib
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/xmpp/base64/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/xmpp/jid/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src/xmpp/xmpp-core/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libiris/src
)

set (LIBRARIES 3rdparty ${QCA2_LIBRARIES} ${IDN_LIBRARIES} ${ZLIB_LIBRARIES} ${QT_LIBRARIES})
if (WIN32)
	set (LIBRARIES ${LIBRARIES} advapi32 ws2_32)
endif (WIN32)

link_directories (${CMAKE_CURRENT_BUILD_DIR}/3rdparty ${QCA2_LIBRARY_DIRS} ${IDN_LIBRARY_DIRS} ${ZLIB_LIBRARY_DIRS})

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
