#find_package(Automoc4 REQUIRED)
include(FindQt4)

find_package(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

# based on qt4_automoc
MACRO(KADU_JABBER_AUTOMOC)
  QT4_GET_MOC_FLAGS(_moc_INCS)

  SET(_matching_FILES )
  FOREACH (_current_FILE ${ARGN})

      GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
      # if "SKIP_AUTOMOC" is set to true, we will not handle this file here.
      # This is required to make uic work correctly:
      # we need to add generated .cpp files to the sources (to compile them),
      # but we cannot let automoc handle them, as the .cpp files don't exist yet when
      # cmake is run for the very first time on them -> however the .cpp files might
      # exist at a later run. at that time we need to skip them, so that we don't add two
      # different rules for the same moc file
      GET_SOURCE_FILE_PROPERTY(_skip ${_abs_FILE} SKIP_AUTOMOC)

      IF ( NOT _skip AND EXISTS ${_abs_FILE} )

	FILE(READ ${_abs_FILE} _contents)

	GET_FILENAME_COMPONENT(_abs_PATH ${_abs_FILE} PATH)

	STRING(REGEX MATCHALL "#include +[^ ]+\\.moc[\">]" _match "${_contents}")
	IF(_match)
	    FOREACH (_current_MOC_INC ${_match})
	      STRING(REGEX MATCH "[^ <\"]+\\.moc" _current_MOC "${_current_MOC_INC}")
	      GET_FILENAME_COMPONENT(_basename ${_current_MOC} NAME_WE)
	      SET(_header ${_abs_FILE})
	      SET(_moc    ${_abs_PATH}/${_current_MOC})
	      QT4_CREATE_MOC_COMMAND(${_header} ${_moc} "${_moc_INCS}" "")
	      MACRO_ADD_FILE_DEPENDENCIES(${_abs_FILE} ${_moc})
	    ENDFOREACH (_current_MOC_INC)
	ENDIF(_match)
      ENDIF ( NOT _skip AND EXISTS ${_abs_FILE} )
  ENDFOREACH (_current_FILE)
ENDMACRO(KADU_JABBER_AUTOMOC)

pkg_search_module (QCA2 REQUIRED qca2 )
pkg_search_module (IDN REQUIRED libidn )

set(SOURCES_IRIS
	libiris/iris/xmpp/base/randomnumbergenerator.cpp
	libiris/iris/xmpp/jid/jid.cpp
	libiris/iris/xmpp/cutestuff/httppoll.cpp
	libiris/iris/xmpp/cutestuff/socks.cpp
	libiris/iris/xmpp/cutestuff/bytestream.cpp
	libiris/iris/xmpp/cutestuff/bsocket.cpp
	libiris/iris/xmpp/cutestuff/httpconnect.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_discoitem.cpp
	libiris/iris/xmpp/xmpp-im/client.cpp
	libiris/iris/xmpp/xmpp-im/types.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_vcard.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_xmlcommon.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_ibb.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_xdata.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_task.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_features.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_discoinfotask.cpp
	libiris/iris/xmpp/xmpp-im/s5b.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_tasks.cpp
	libiris/iris/xmpp/xmpp-im/filetransfer.cpp
	libiris/iris/xmpp/sasl/digestmd5proplist.cpp
	libiris/iris/xmpp/sasl/digestmd5response.cpp
	libiris/iris/xmpp/sasl/plainmessage.cpp
	libiris/iris/xmpp/zlib/zlibcompressor.cpp
	libiris/iris/xmpp/zlib/zlibdecompressor.cpp
	libiris/iris/xmpp/base64/base64.cpp
	libiris/iris/xmpp/xmpp-core/tlshandler.cpp
	libiris/iris/xmpp/xmpp-core/xmpp_stanza.cpp
	libiris/iris/xmpp/xmpp-core/stream.cpp
	libiris/iris/xmpp/xmpp-core/securestream.cpp
	libiris/iris/xmpp/xmpp-core/simplesasl.cpp
	libiris/iris/xmpp/xmpp-core/xmlprotocol.cpp
	libiris/iris/xmpp/xmpp-core/protocol.cpp
	libiris/iris/xmpp/xmpp-core/compressionhandler.cpp
	libiris/iris/xmpp/xmpp-core/parser.cpp
	libiris/iris/xmpp/xmpp-core/connector.cpp
	libiris/iris/jdns/qjdns.cpp
	libiris/iris/jdns/qjdns_sock.cpp
	libiris/iris/irisnet/noncore/ice176.cpp
	libiris/iris/irisnet/noncore/stunallocate.cpp
	libiris/iris/irisnet/noncore/legacy/ndns.cpp
	libiris/iris/irisnet/noncore/legacy/srvresolver.cpp
	libiris/iris/irisnet/noncore/legacy/safedelete.cpp
	libiris/iris/irisnet/noncore/legacy/servsock.cpp
	libiris/iris/irisnet/noncore/icelocaltransport.cpp
	libiris/iris/irisnet/noncore/stunmessage.cpp
	libiris/iris/irisnet/noncore/stunbinding.cpp
	libiris/iris/irisnet/noncore/stuntransaction.cpp
	libiris/iris/irisnet/noncore/processquit.cpp
	libiris/iris/irisnet/corelib/netavailability.cpp
	libiris/iris/irisnet/corelib/netnames_jdns.cpp
	libiris/iris/irisnet/corelib/netnames.cpp
	libiris/iris/irisnet/corelib/irisnetplugin.cpp
	libiris/iris/irisnet/corelib/netinterface.cpp
	libiris/iris/irisnet/corelib/jdnsshared.cpp
	libiris/iris/irisnet/corelib/objectsession.cpp
	libiris/iris/irisnet/corelib/irisnetglobal.cpp

	libiris/iris/jdns/jdns.c
	libiris/iris/jdns/jdns_util.c
	libiris/iris/jdns/jdns_sys.c
	libiris/iris/jdns/jdns_mdnsd.c
	libiris/iris/jdns/jdns_packet.c

	libiris/iris/xmpp/jingle/jinglecontent.cpp
	libiris/iris/xmpp/jingle/jinglesession.cpp
	libiris/iris/xmpp/jingle/jinglesessionmanager.cpp
	libiris/iris/xmpp/jingle/jingletasks.cpp
)

if(WIN32)
  LIST(APPEND SOURCES_IRIS libiris/iris/irisnet/corelib/netinterface_win.cpp)
else(WIN32)
  LIST(APPEND SOURCES_IRIS libiris/iris/irisnet/corelib/netinterface_unix.cpp)
endif(WIN32)

set (MOC_SOURCES_IRIS
	libiris/iris/xmpp/base/randrandomnumbergenerator.h
	libiris/iris/xmpp/base/randomnumbergenerator.h
	libiris/iris/xmpp/jid/jid.h
	libiris/iris/xmpp/cutestuff/bytestream.h
	libiris/iris/xmpp/cutestuff/socks.h
	libiris/iris/xmpp/cutestuff/httpconnect.h
	libiris/iris/xmpp/cutestuff/bsocket.h
	libiris/iris/xmpp/cutestuff/httppoll.h
	libiris/iris/xmpp/xmpp-im/xmpp_xdata.h
	libiris/iris/xmpp/xmpp-im/xmpp_rosterx.h
	libiris/iris/xmpp/xmpp-im/xmpp_tasks.h
	libiris/iris/xmpp/xmpp-im/xmpp_discoitem.h
	libiris/iris/xmpp/xmpp-im/xmpp_pubsubretraction.h
	libiris/iris/xmpp/xmpp-im/xmpp_xmlcommon.h
	libiris/iris/xmpp/xmpp-im/xmpp_roster.h
	libiris/iris/xmpp/xmpp-im/xmpp_muc.h
	libiris/iris/xmpp/xmpp-im/xmpp_htmlelement.h
	libiris/iris/xmpp/xmpp-im/xmpp_receipts.h
	libiris/iris/xmpp/xmpp-im/xmpp_message.h
	libiris/iris/xmpp/xmpp-im/xmpp_discoinfotask.h
	libiris/iris/xmpp/xmpp-im/xmpp_ibb.h
	libiris/iris/xmpp/xmpp-im/xmpp_liverosteritem.h
	libiris/iris/xmpp/xmpp-im/xmpp_httpauthrequest.h
	libiris/iris/xmpp/xmpp-im/xmpp_pubsubitem.h
	libiris/iris/xmpp/xmpp-im/xmpp_vcard.h
	libiris/iris/xmpp/xmpp-im/xmpp_client.h
	libiris/iris/xmpp/xmpp-im/s5b.h
	libiris/iris/xmpp/xmpp-im/xmpp_url.h
	libiris/iris/xmpp/xmpp-im/xmpp_chatstate.h
	libiris/iris/xmpp/xmpp-im/xmpp_task.h
	libiris/iris/xmpp/xmpp-im/xmpp_address.h
	libiris/iris/xmpp/xmpp-im/xmpp_agentitem.h
	libiris/iris/xmpp/xmpp-im/filetransfer.h
	libiris/iris/xmpp/xmpp-im/xmpp_status.h
	libiris/iris/xmpp/xmpp-im/xmpp_resourcelist.h
	libiris/iris/xmpp/xmpp-im/xmpp_liveroster.h
	libiris/iris/xmpp/xmpp-im/xmpp_resource.h
	libiris/iris/xmpp/xmpp-im/xmpp_rosteritem.h
	libiris/iris/xmpp/xmpp-im/xmpp_features.h
	libiris/iris/xmpp/xmpp-im/im.h
	libiris/iris/xmpp/sasl/digestmd5response.h
	libiris/iris/xmpp/sasl/digestmd5proplist.h
	libiris/iris/xmpp/sasl/plainmessage.h
	libiris/iris/xmpp/zlib/common.h
	libiris/iris/xmpp/zlib/zlibdecompressor.h
	libiris/iris/xmpp/zlib/zlibcompressor.h
	libiris/iris/xmpp/base64/base64.h
	libiris/iris/xmpp/xmpp-core/xmpp_stanza.h
	libiris/iris/xmpp/xmpp-core/simplesasl.h
	libiris/iris/xmpp/xmpp-core/securestream.h
	libiris/iris/xmpp/xmpp-core/protocol.h
	libiris/iris/xmpp/xmpp-core/compressionhandler.h
	libiris/iris/xmpp/xmpp-core/xmlprotocol.h
	libiris/iris/xmpp/xmpp-core/xmpp.h
	libiris/iris/xmpp/xmpp-core/xmpp_clientstream.h
	libiris/iris/xmpp/xmpp-core/xmpp_stream.h
	libiris/iris/xmpp/xmpp-core/td.h
	libiris/iris/xmpp/xmpp-core/parser.h

	libiris/iris/jdns/qjdns.h

	libiris/iris/irisnet/noncore/stuntransaction.h
	libiris/iris/irisnet/noncore/stunbinding.h
	libiris/iris/irisnet/noncore/legacy/ndns.h
	libiris/iris/irisnet/noncore/legacy/safedelete.h
	libiris/iris/irisnet/noncore/legacy/servsock.h
	libiris/iris/irisnet/noncore/legacy/srvresolver.h
	libiris/iris/irisnet/noncore/stunallocate.h
	libiris/iris/irisnet/noncore/ice176.h
	libiris/iris/irisnet/noncore/processquit.h
	libiris/iris/irisnet/noncore/icelocaltransport.h
	libiris/iris/irisnet/corelib/irisnetglobal.h
	libiris/iris/irisnet/corelib/objectsession.h
	libiris/iris/irisnet/corelib/netavailability.h
	libiris/iris/irisnet/corelib/netnames.h
	libiris/iris/irisnet/corelib/irisnetplugin.h
	libiris/iris/irisnet/corelib/netinterface.h
	libiris/iris/irisnet/corelib/jdnsshared.h

	libiris/iris/xmpp/jingle/jinglecontent.h
	libiris/iris/xmpp/jingle/jinglesession.h
	libiris/iris/xmpp/jingle/jinglesessionmanager.h
	libiris/iris/xmpp/jingle/jingletasks.h
)

set (MOC_SOURCES2_IRIS
	libiris/iris/jdns/qjdns.cpp
	libiris/iris/irisnet/corelib/netavailability.cpp
	libiris/iris/irisnet/corelib/jdnsshared.cpp
	libiris/iris/irisnet/corelib/netinterface.cpp
	libiris/iris/irisnet/corelib/netnames.cpp
	libiris/iris/irisnet/corelib/netnames_jdns.cpp
	libiris/iris/irisnet/corelib/objectsession.cpp
	libiris/iris/irisnet/noncore/ice176.cpp
	libiris/iris/irisnet/noncore/icelocaltransport.cpp
	libiris/iris/irisnet/noncore/processquit.cpp
	libiris/iris/irisnet/noncore/stunallocate.cpp
	libiris/iris/irisnet/noncore/stunbinding.cpp
	libiris/iris/irisnet/noncore/stuntransaction.cpp
	libiris/iris/xmpp/cutestuff/bsocket.cpp
	libiris/iris/xmpp/xmpp-core/securestream.cpp
	libiris/iris/xmpp/xmpp-im/s5b.cpp
	libiris/iris/xmpp/xmpp-im/xmpp_features.cpp
)

if(WIN32)
  LIST(APPEND MOC_SOURCES2_IRIS libiris/iris/irisnet/corelib/netinterface_win.cpp)
else(WIN32)
  LIST(APPEND MOC_SOURCES2_IRIS libiris/iris/irisnet/corelib/netinterface_unix.cpp)
endif(WIN32)


set (SOURCES
	file-transfer/jabber-file-transfer.cpp
	gui/widgets/jabber-create-account-widget.cpp
	gui/widgets/jabber-edit-account-widget.cpp
	gui/widgets/jabber-personal-info-widget.cpp
	server/jabber-server-register-account.cpp
	services/jabber-chat-service.cpp
	services/jabber-file-transfer-service.cpp
	jabber-client.cpp
	jabber-account-data-manager.cpp
	jabber-configuration-dialog.cpp
	jabber-contact-account-data.cpp
	jabber-contact-account-data-manager.cpp
	jabber-contact-account-data-widget.cpp
	jabber-account.cpp
	jabber_protocol.cpp
	jabber-protocol-factory.cpp
	jabber-resource.cpp
	jabber-resource-pool.cpp
	jabber-transport.cpp
	jid-util.cpp
	miniclient.cpp
	system-info.cpp
	cert-util.cpp
	ssl-cert-dlg.cpp
)

set (MOC_SOURCES
	file-transfer/jabber-file-transfer.h
	gui/widgets/jabber-create-account-widget.h
	gui/widgets/jabber-edit-account-widget.h
	gui/widgets/jabber-personal-info-widget.h
	server/jabber-server-register-account.h
	services/jabber-chat-service.h
	services/jabber-file-transfer-service.h
	jabber-client.h
	jabber-configuration-dialog.h
	#jabber-contact-account-data.h
	#jabber-contact-account-data-manager.h
	jabber-contact-account-data-widget.h
	#jabber-account.h
	jabber_protocol.h
	#jabber-protocol-factory.h
	jabber-resource.h
	jabber-resource-pool.h
	jabber-transport.h
	miniclient.h
	#jid-util.cpp
	system-info.h
	#cert-util.h
	ssl-cert-dlg.h
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
	configuration/jabber_contact.ui
	configuration/jabber_protocol.ui
)

KADU_JABBER_AUTOMOC ( ${MOC_SOURCES2_IRIS} )

link_directories( ${QCA2_LIBRARY_DIRS} ${IDN_LIBRARY_DIRS} )
add_definitions( ${QCA2_CFLAGS_OTHER} ${IDN_CFLAGS_OTHER} )

kadu_module (jabber_protocol
	MODULE_SOURCES ${SOURCES_IRIS}  ${SOURCES}
	MODULE_MOC_SOURCES  ${MOC_SOURCES_IRIS} ${MOC_SOURCES}
#	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

set_target_properties(jabber_protocol PROPERTIES
	link_flags "${QCA2_LDFLAGS_OTHER} ${IDN_LDFLAGS_OTHER}" )

include_directories(
	${QCA2_INCLUDE_DIRS}
	${IDN_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/libiris
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/include
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/base
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/base
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/jid
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/cutestuff
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/xmpp-im
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/sasl
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/zlib
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/base64
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/xmpp-core
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/xmpp/jingle
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/jdns
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/irisnet/noncore
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/irisnet/noncore/legacy
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/irisnet/corelib
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/iris/irisnet/appledns
)

target_link_libraries(jabber_protocol ${QCA2_LIBRARIES} ${IDN_LIBRARIES} ${QT_LIBRARIES} )

install (FILES ${DATA_FILES}
        DESTINATION share/kadu/modules/data/jabber_protocol)
