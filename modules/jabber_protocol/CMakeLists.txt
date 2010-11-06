#find_package (Automoc4 REQUIRED)
include (FindQt4)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

# based on qt4_automoc
macro (KADU_JABBER_AUTOMOC)
	QT4_GET_MOC_FLAGS (_moc_INCS)

	set (_matching_FILES)
	foreach (_current_FILE ${ARGN})
		get_filename_component (_abs_FILE ${_current_FILE} ABSOLUTE)
		# if "SKIP_AUTOMOC" is set to true, we will not handle this file here.
		# This is required to make uic work correctly:
		# we need to add generated .cpp files to the sources (to compile them),
		# but we cannot let automoc handle them, as the .cpp files don't exist yet when
		# cmake is run for the very first time on them -> however the .cpp files might
		# exist at a later run. at that time we need to skip them, so that we don't add two
		# different rules for the same moc file
		get_source_file_property (_skip ${_abs_FILE} SKIP_AUTOMOC)

		if (NOT _skip AND EXISTS ${_abs_FILE})
			file (READ ${_abs_FILE} _contents)

			get_filename_component (_abs_PATH ${_abs_FILE} PATH)

			string (REGEX MATCHALL "#include +[^ ]+\\.moc[\">]" _match "${_contents}")
			if (_match)
				foreach (_current_MOC_INC ${_match})
					string (REGEX MATCH "[^ <\"]+\\.moc" _current_MOC "${_current_MOC_INC}")
					get_filename_component (_basename ${_current_MOC} NAME_WE)
					set (_header ${_abs_FILE})
					set (_moc ${_abs_PATH}/${_current_MOC})
					QT4_CREATE_MOC_COMMAND (${_header} ${_moc} "${_moc_INCS}" "")
					MACRO_ADD_FILE_DEPENDENCIES (${_abs_FILE} ${_moc})
				endforeach (_current_MOC_INC)
			endif (_match)
		endif (NOT _skip AND EXISTS ${_abs_FILE})
	endforeach (_current_FILE)
endmacro (KADU_JABBER_AUTOMOC)

if (MINGW)
	if (NOT MINGW_QCA_DIR)
		message (SEND_ERROR " Pass path to libqca2 -DMINGW_QCA_DIR=path")
	else (NOT MINGW_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${MINGW_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARY_DIRS ${MINGW_QCA_DIR}/bin)
		set (QCA2_LIBRARIES -lqca2)
	endif (NOT MINGW_QCA_DIR)
	if (NOT MINGW_ZLIB_DIR)
		message (SEND_ERROR " Pass path to zlib -DMINGW_ZLIB_DIR=path")
	else (NOT MINGW_ZLIB_DIR)
		set (ZLIB_INCLUDE_DIRS ${MINGW_ZLIB_DIR})
		set (ZLIB_LIBRARY_DIRS ${MINGW_ZLIB_DIR})
		set (ZLIB_LIBRARIES -lzlib1)
	endif (NOT MINGW_ZLIB_DIR)
	if (NOT MINGW_IDN_DIR)
		message (SEND_ERROR " Pass path to libidn -DMINGW_IDN_DIR=path")
	else (NOT MINGW_IDN_DIR)
		set (IDN_INCLUDE_DIRS ${MINGW_IDN_DIR})
		set (IDN_LIBRARY_DIRS ${MINGW_IDN_DIR})
		set (IDN_LIBRARIES -lidn)
	endif (NOT MINGW_IDN_DIR)
else (MINGW)
	pkg_search_module (QCA2 REQUIRED qca2)
	pkg_search_module (IDN REQUIRED libidn)

	include (FindZLIB)
	if (NOT ZLIB_FOUND)
		pkg_search_module (ZLIB REQUIRED zlib)
	endif (NOT ZLIB_FOUND)
endif (MINGW)

set (SOURCES_IRIS
	libiris/src/xmpp/base/randomnumbergenerator.cpp
	libiris/src/xmpp/jid/jid.cpp
	libiris/src/irisnet/noncore/cutestuff/httppoll.cpp
	libiris/src/irisnet/noncore/cutestuff/socks.cpp
	libiris/src/irisnet/noncore/cutestuff/bytestream.cpp
	libiris/src/irisnet/noncore/cutestuff/bsocket.cpp
	libiris/src/irisnet/noncore/cutestuff/httpconnect.cpp
	libiris/src/xmpp/xmpp-im/xmpp_discoitem.cpp
	libiris/src/xmpp/xmpp-im/client.cpp
	libiris/src/xmpp/xmpp-im/types.cpp
	libiris/src/xmpp/xmpp-im/xmpp_vcard.cpp
	libiris/src/xmpp/xmpp-im/xmpp_xmlcommon.cpp
	libiris/src/xmpp/xmpp-im/xmpp_ibb.cpp
	libiris/src/xmpp/xmpp-im/xmpp_xdata.cpp
	libiris/src/xmpp/xmpp-im/xmpp_task.cpp
	libiris/src/xmpp/xmpp-im/xmpp_features.cpp
	libiris/src/xmpp/xmpp-im/xmpp_discoinfotask.cpp
	libiris/src/xmpp/xmpp-im/s5b.cpp
	libiris/src/xmpp/xmpp-im/xmpp_tasks.cpp
	libiris/src/xmpp/xmpp-im/filetransfer.cpp
	libiris/src/xmpp/sasl/digestmd5proplist.cpp
	libiris/src/xmpp/sasl/digestmd5response.cpp
	libiris/src/xmpp/sasl/plainmessage.cpp
	libiris/src/xmpp/zlib/zlibcompressor.cpp
	libiris/src/xmpp/zlib/zlibdecompressor.cpp
	libiris/src/xmpp/base64/base64.cpp
	libiris/src/xmpp/xmpp-core/tlshandler.cpp
	libiris/src/xmpp/xmpp-core/xmpp_stanza.cpp
	libiris/src/xmpp/xmpp-core/stream.cpp
	libiris/src/xmpp/xmpp-core/securestream.cpp
	libiris/src/xmpp/xmpp-core/simplesasl.cpp
	libiris/src/xmpp/xmpp-core/xmlprotocol.cpp
	libiris/src/xmpp/xmpp-core/protocol.cpp
	libiris/src/xmpp/xmpp-core/compressionhandler.cpp
	libiris/src/xmpp/xmpp-core/parser.cpp
	libiris/src/xmpp/xmpp-core/connector.cpp
	libiris/src/jdns/qjdns.cpp
	libiris/src/jdns/qjdns_sock.cpp
	libiris/src/irisnet/noncore/ice176.cpp
	libiris/src/irisnet/noncore/stunallocate.cpp
	libiris/src/irisnet/noncore/legacy/ndns.cpp
	libiris/src/irisnet/noncore/legacy/srvresolver.cpp
	libiris/src/irisnet/noncore/legacy/safedelete.cpp
	libiris/src/irisnet/noncore/legacy/servsock.cpp
	libiris/src/irisnet/noncore/icecomponent.cpp
	libiris/src/irisnet/noncore/icetransport.cpp
	libiris/src/irisnet/noncore/iceturntransport.cpp
	libiris/src/irisnet/noncore/icelocaltransport.cpp
	libiris/src/irisnet/noncore/stunmessage.cpp
	libiris/src/irisnet/noncore/stunbinding.cpp
	libiris/src/irisnet/noncore/stuntransaction.cpp
	libiris/src/irisnet/noncore/stuntypes.cpp
	libiris/src/irisnet/noncore/stunutil.cpp
	libiris/src/irisnet/noncore/processquit.cpp
	libiris/src/irisnet/noncore/turnclient.cpp
	libiris/src/irisnet/noncore/udpportreserver.cpp
	libiris/src/irisnet/corelib/addressresolver.cpp
	libiris/src/irisnet/corelib/netavailability.cpp
	libiris/src/irisnet/corelib/netnames_jdns.cpp
	libiris/src/irisnet/corelib/netnames.cpp
	libiris/src/irisnet/corelib/irisnetplugin.cpp
	libiris/src/irisnet/corelib/netinterface.cpp
	libiris/src/irisnet/corelib/jdnsshared.cpp
	libiris/src/irisnet/corelib/objectsession.cpp
	libiris/src/irisnet/corelib/irisnetglobal.cpp

	libiris/src/jdns/jdns.c
	libiris/src/jdns/jdns_util.c
	libiris/src/jdns/jdns_sys.c
	libiris/src/jdns/jdns_mdnsd.c
	libiris/src/jdns/jdns_packet.c
)

if (WIN32)
	list (APPEND SOURCES_IRIS libiris/src/irisnet/corelib/netinterface_win.cpp)
else (WIN32)
	list (APPEND SOURCES_IRIS libiris/src/irisnet/corelib/netinterface_unix.cpp)
endif (WIN32)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

set (MOC_SOURCES_IRIS
	libiris/src/irisnet/noncore/cutestuff/bytestream.h
	libiris/src/irisnet/noncore/cutestuff/socks.h
	libiris/src/irisnet/noncore/cutestuff/httpconnect.h
	libiris/src/irisnet/noncore/cutestuff/bsocket.h
	libiris/src/irisnet/noncore/cutestuff/httppoll.h
	libiris/src/xmpp/xmpp-im/xmpp_tasks.h
	libiris/src/xmpp/xmpp-im/xmpp_discoinfotask.h
	libiris/src/xmpp/xmpp-im/xmpp_ibb.h
	libiris/src/xmpp/xmpp-im/xmpp_client.h
	libiris/src/xmpp/xmpp-im/s5b.h
	libiris/src/xmpp/xmpp-im/xmpp_task.h
	libiris/src/xmpp/xmpp-im/filetransfer.h
	libiris/src/xmpp/zlib/zlibdecompressor.h
	libiris/src/xmpp/zlib/zlibcompressor.h
	libiris/src/xmpp/xmpp-core/securestream.h
	libiris/src/xmpp/xmpp-core/compressionhandler.h
	libiris/src/xmpp/xmpp-core/xmpp.h
	libiris/src/xmpp/xmpp-core/xmpp_clientstream.h
	libiris/src/xmpp/xmpp-core/xmpp_stream.h

	libiris/src/jdns/qjdns.h

	libiris/src/irisnet/noncore/stuntransaction.h
	libiris/src/irisnet/noncore/stunbinding.h
	libiris/src/irisnet/noncore/legacy/ndns.h
	libiris/src/irisnet/noncore/legacy/safedelete.h
	libiris/src/irisnet/noncore/legacy/servsock.h
	libiris/src/irisnet/noncore/legacy/srvresolver.h
	libiris/src/irisnet/noncore/stunallocate.h
	libiris/src/irisnet/noncore/ice176.h
	libiris/src/irisnet/noncore/icecomponent.h
	libiris/src/irisnet/noncore/processquit.h
	libiris/src/irisnet/noncore/icetransport.h
	libiris/src/irisnet/noncore/iceturntransport.h
	libiris/src/irisnet/noncore/icelocaltransport.h
	libiris/src/irisnet/noncore/turnclient.h
	libiris/src/irisnet/noncore/udpportreserver.h
	libiris/src/irisnet/corelib/addressresolver.h
	libiris/src/irisnet/corelib/irisnetplugin.h
	libiris/src/irisnet/corelib/jdnsshared.h
	libiris/src/irisnet/corelib/netavailability.h
	libiris/src/irisnet/corelib/netinterface.h
	libiris/src/irisnet/corelib/netnames.h
	libiris/src/irisnet/corelib/objectsession.h
)

set (MOC_SOURCES2_IRIS
	libiris/src/jdns/qjdns.cpp
	libiris/src/irisnet/corelib/addressresolver.cpp
	libiris/src/irisnet/corelib/jdnsshared.cpp
	libiris/src/irisnet/corelib/netavailability.cpp
	libiris/src/irisnet/corelib/netinterface.cpp
	libiris/src/irisnet/corelib/netnames.cpp
	libiris/src/irisnet/corelib/netnames_jdns.cpp
	libiris/src/irisnet/corelib/objectsession.cpp
	libiris/src/irisnet/noncore/ice176.cpp
	libiris/src/irisnet/noncore/icecomponent.cpp
	libiris/src/irisnet/noncore/icelocaltransport.cpp
	libiris/src/irisnet/noncore/icetransport.cpp
	libiris/src/irisnet/noncore/iceturntransport.cpp
	libiris/src/irisnet/noncore/processquit.cpp
	libiris/src/irisnet/noncore/stunallocate.cpp
	libiris/src/irisnet/noncore/stunbinding.cpp
	libiris/src/irisnet/noncore/stuntransaction.cpp
	libiris/src/irisnet/noncore/stuntypes.cpp
	libiris/src/irisnet/noncore/turnclient.cpp
	libiris/src/irisnet/noncore/udpportreserver.cpp
	libiris/src/irisnet/noncore/cutestuff/bsocket.cpp
	libiris/src/xmpp/xmpp-core/securestream.cpp
	libiris/src/xmpp/xmpp-im/s5b.cpp
	libiris/src/xmpp/xmpp-im/xmpp_features.cpp
)

if (WIN32)
	list (APPEND MOC_SOURCES2_IRIS libiris/src/irisnet/corelib/netinterface_win.cpp)
else (WIN32)
	list (APPEND MOC_SOURCES2_IRIS libiris/src/irisnet/corelib/netinterface_unix.cpp)
endif (WIN32)

set (SOURCES
	certificates/certificate-display-dialog.cpp
	certificates/certificate-error-dialog.cpp
	certificates/certificate-helpers.cpp
	certificates/trusted-certificates-manager.cpp
	client/jabber-client.cpp
	client/mini-client.cpp
	file-transfer/jabber-file-transfer-handler.cpp
	gui/widgets/jabber-contact-personal-info-widget.cpp
	gui/widgets/jabber-add-account-widget.cpp
	gui/widgets/jabber-create-account-widget.cpp
	gui/widgets/jabber-edit-account-widget.cpp
	gui/widgets/jabber-personal-info-widget.cpp
	gui/windows/jabber-change-password-window.cpp
	gui/windows/xml-console.cpp
	open-chat-with/jabber-open-chat-with-runner.cpp
	iris-status-adapter.cpp
	jabber-account-data-manager.cpp
	jabber-account-details.cpp
	jabber-contact-data-manager.cpp
	jabber-contact-details.cpp
	jabber-module.cpp
	jabber-protocol.cpp
	jabber-protocol-factory.cpp
	jabber-url-handler.cpp
	facebook-protocol-factory.cpp
	gtalk-protocol-factory.cpp
	resource/jabber-resource.cpp
	resource/jabber-resource-pool.cpp
	server/jabber-avatar-fetcher.cpp
	server/jabber-server-change-password.cpp
	server/jabber-server-register-account.cpp
	services/jabber-avatar-service.cpp
	services/jabber-chat-service.cpp
	services/jabber-chat-state-service.cpp
	services/jabber-file-transfer-service.cpp
	services/jabber-personal-info-service.cpp
	services/jabber-contact-personal-info-service.cpp
	services/jabber-roster-service.cpp
	services/jabber-subscription-service.cpp
	transport/jabber-transport.cpp
	utils/jid-util.cpp
	utils/vcard-factory.cpp
	utils/pep-manager.cpp
	utils/server-info-manager.cpp
)

set (MOC_SOURCES
	certificates/certificate-display-dialog.h
	certificates/trusted-certificates-manager.h
	client/jabber-client.h
	client/mini-client.h
	facebook-protocol-factory.h
	file-transfer/jabber-file-transfer-handler.h
	gtalk-protocol-factory.h
	gui/widgets/jabber-contact-personal-info-widget.h
	gui/widgets/jabber-add-account-widget.h
	gui/widgets/jabber-create-account-widget.h
	gui/widgets/jabber-edit-account-widget.h
	gui/widgets/jabber-personal-info-widget.h
	gui/windows/jabber-change-password-window.h
	gui/windows/xml-console.h
	jabber-account-data-manager.h
	jabber-contact-data-manager.h
	jabber-protocol.h
	jabber-protocol-factory.h
	jabber-url-handler.h
	resource/jabber-resource.h
	resource/jabber-resource-pool.h
	server/jabber-avatar-fetcher.h
	server/jabber-server-change-password.h
	server/jabber-server-register-account.h
	services/jabber-avatar-service.h
	services/jabber-chat-service.h
	services/jabber-chat-state-service.h
	services/jabber-file-transfer-service.h
	services/jabber-personal-info-service.h
	services/jabber-contact-personal-info-service.h
	services/jabber-roster-service.h
	services/jabber-subscription-service.h
	transport/jabber-transport.h
	utils/vcard-factory.h
	utils/pep-manager.h
	utils/server-info-manager.h
)

set (DATA_FILES
	data/status/away.png
	data/status/ffc.png
	data/status/dnd.png
	data/status/invisible.png
	data/status/offline.png
	data/status/online.png
	data/status/xa.png
)

set (TRANSLATION_SOURCES
	translations/jabber_protocol_pl.ts
)

set (CONFIGURATION_FILES
)

KADU_JABBER_AUTOMOC (${MOC_SOURCES2_IRIS})

link_directories (${QCA2_LIBRARY_DIRS} ${IDN_LIBRARY_DIRS} ${ZLIB_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER} ${IDN_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER} -DIRISNET_STATIC)

kadu_module (jabber_protocol
	MODULE_SOURCES ${SOURCES_IRIS}  ${SOURCES}
	MODULE_MOC_SOURCES  ${MOC_SOURCES_IRIS} ${MOC_SOURCES}
	MODULE_DEPENDENCIES ${QCA2_LIBRARIES} ${IDN_LIBRARIES} ${ZLIB_LIBRARIES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

#set_target_properties (jabber_protocol PROPERTIES
#	link_flags "${QCA2_LDFLAGS_OTHER} ${IDN_LDFLAGS_OTHER} ${ZLIB_LDFLAGS_OTHER}"
#)

include_directories (
	${QCA2_INCLUDE_DIRS}
	${IDN_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/libiris
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/include
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/base
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/base
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/jid
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/noncore/cutestuff/
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/xmpp-im
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/sasl
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/zlib
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/base64
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/xmpp-core
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/jingle
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/jdns
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/noncore
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/noncore/legacy
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/corelib
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/appledns
)

target_link_libraries (jabber_protocol ${QCA2_LIBRARIES} ${IDN_LIBRARIES} ${ZLIB_LIBRARIES} ${QT_LIBRARIES})

install (FILES ${DATA_FILES}
	DESTINATION ${KADU_MODULES_DIR}/data/jabber_protocol
)
