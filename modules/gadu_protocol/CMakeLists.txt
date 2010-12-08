set (LIBGADU_MIN_VERSION "1.9.0")

set (SOURCES
	dcc/dcc-manager.cpp
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

	server/gadu-avatar-data-parser.cpp
	server/gadu-avatar-fetcher.cpp
	server/gadu-avatar-uploader.cpp
	server/gadu-contact-list-handler.cpp
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
	services/gadu-contact-personal-info-service.cpp
	services/gadu-search-service.cpp

	socket-notifiers/gadu-protocol-socket-notifiers.cpp
	socket-notifiers/gadu-pubdir-socket-notifiers.cpp
	socket-notifiers/gadu-socket-notifiers.cpp
	socket-notifiers/gadu-token-socket-notifiers.cpp

	gadu-account-data-manager.cpp
	gadu-account-details.cpp
	gadu-contact-data-manager.cpp
	gadu-contact-details.cpp
	gadu-id-validator.cpp
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
	server/gadu-contact-list-handler.h
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
	services/gadu-contact-personal-info-service.h
	services/gadu-search-service.h

	socket-notifiers/gadu-protocol-socket-notifiers.h
	socket-notifiers/gadu-pubdir-socket-notifiers.h
	socket-notifiers/gadu-socket-notifiers.h
	socket-notifiers/gadu-token-socket-notifiers.h

	gadu-account-data-manager.h
	#gadu-account-details.h
	gadu-contact-data-manager.h
	gadu-id-validator.h
	gadu-protocol-factory.h
	gadu-protocol.h
	gadu-resolver.h
	gadu-url-handler.h
)

set (TRANSLATION_SOURCES
#	translations/gadu_protocol_de.ts
#	translations/gadu_protocol_en.ts
#	translations/gadu_protocol_fr.ts
#	translations/gadu_protocol_it.ts
	translations/gadu_protocol_pl.ts
)

set (CONFIGURATION_FILES
)

set (SERVERS_FILE
	data/servers.txt
)

if (MINGW)
	if (NOT MINGW_LIBGADU_DIR)
		message (SEND_ERROR " Pass path to libgadu -DMINGW_LIBGADU_DIR=path")
		set (MINGW_LIBGADU_DIR "")
	else (NOT MINGW_LIBGADU_DIR)
		set (LIBGADU_FOUND true)
		set (LIBGADU_INCLUDE_DIRS ${MINGW_LIBGADU_DIR})
		set (LIBGADU_LIBRARY_DIRS ${MINGW_LIBGADU_DIR})
		set (LIBGADU_LIBRARIES -lgadu -lws2_32)
	endif (NOT MINGW_LIBGADU_DIR)
	if (NOT MINGW_QCA_DIR)
		message (SEND_ERROR " Pass path to libsndfile -DMINGW_QCA_DIR=path")
		set (MINGW_LIBSNDFILE_DIR "")
	else (NOT MINGW_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${MINGW_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARY_DIRS ${MINGW_QCA_DIR}/bin)
		set (QCA2_LIBRARIES -lqca2)
	endif (NOT MINGW_QCA_DIR)
else (MINGW)
	pkg_search_module (QCA2 REQUIRED qca2)
	pkg_search_module (LIBGADU REQUIRED libgadu>=${LIBGADU_MIN_VERSION})
endif (MINGW)

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})

include_directories (${LIBGADU_INCLUDE_DIRS} ${QCA2_INCLUDE_DIRS})
link_directories (${LIBGADU_LIBRARY_DIRS})

if (LIBGADU_FOUND)
	message (STATUS " * libgadu (version ${LIBGADU_VERSION}) was found in ${LIBGADU_LIBDIR}")
else (LIBGADU_FOUND)
	message (FATAL_ERROR " * libgadu (version ${LIBGADU_MIN_VERSION}) was NOT FOUND")
endif (LIBGADU_FOUND)

find_package (Qt4 REQUIRED)
set (QT_USE_QTXML 1)
set (QT_USE_QTXMLPATTERNS 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
set (QT_USE_QTSCRIPT 1)
set (QT_USE_QTSCRIPTTOOLS 1)
include (${QT_USE_FILE}) 

kadu_module (gadu_protocol
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${LIBGADU_LIBRARIES} ${QCA2_LIBRARIES} ${QT_LIBRARIES} ${QT_QTXMLPATTERNS_LIBRARY}
	MODULE_DEPENDENCIES ${QCA2_LIBRARIES}
	MODULE_DATA_FILES ${SERVERS_FILE}
)

if (WIN32)
	set_target_properties (gadu_protocol PROPERTIES COMPILE_FLAGS "-DGADU_BUILD")
endif (WIN32)
