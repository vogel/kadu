#find_package(Automoc4 REQUIRED)
include(FindQt4)

### Qt3Support
find_package(Qt4 REQUIRED)
set(QT_USE_QT3SUPPORT 1)
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

set(cutestuff_SRCS
	libiris/cutestuff/legacy/ndns.cpp
	libiris/cutestuff/legacy/safedelete.cpp
	libiris/cutestuff/legacy/servsock.cpp
	libiris/cutestuff/legacy/srvresolver.cpp
	libiris/cutestuff/network/bsocket.cpp
	libiris/cutestuff/network/httpconnect.cpp
	libiris/cutestuff/network/httppoll.cpp
	libiris/cutestuff/network/socks.cpp
	libiris/cutestuff/util/bytestream.cpp
)

set(iris_SRCS
	libiris/iris/jabber/filetransfer.cpp
	libiris/iris/jabber/jinglesessionmanager.cpp
	libiris/iris/jabber/s5b.cpp
	libiris/iris/jabber/xmpp_ibb.cpp
	libiris/iris/jabber/jinglesession.cpp
	libiris/iris/jabber/jingletasks.cpp
	libiris/iris/jabber/jinglecontent.cpp

	libiris/iris/xmpp-core/compress.cpp
	libiris/iris/xmpp-core/compressionhandler.cpp
	libiris/iris/xmpp-core/connector.cpp
	libiris/iris/xmpp-core/jid.cpp
	libiris/iris/xmpp-core/securestream.cpp
	libiris/iris/xmpp-core/tlshandler.cpp
	libiris/iris/xmpp-core/protocol.cpp
	libiris/iris/xmpp-core/stream.cpp
	libiris/iris/xmpp-core/xmlprotocol.cpp
	libiris/iris/xmpp-core/parser.cpp
	libiris/iris/xmpp-core/simplesasl.cpp
	libiris/iris/xmpp-core/xmpp_stanza.cpp

	libiris/iris/xmpp-im/client.cpp
	libiris/iris/xmpp-im/types.cpp
	libiris/iris/xmpp-im/xmpp_tasks.cpp
	libiris/iris/xmpp-im/xmpp_vcard.cpp
	libiris/iris/xmpp-im/xmpp_xmlcommon.cpp
	libiris/iris/xmpp-im/xmpp_xdata.cpp
	libiris/iris/xmpp-im/xmpp_features.cpp
	libiris/iris/xmpp-im/xmpp_discoinfotask.cpp
	libiris/iris/xmpp-im/xmpp_discoitem.cpp
	libiris/iris/xmpp-im/xmpp_task.cpp
)

set(SOURCES_IRIS
	${cutestuff_SRCS}
	${iris_SRCS}
)

set (MOC_SOURCES_IRIS
	libiris/cutestuff/legacy/ndns.h
	libiris/cutestuff/legacy/safedelete.h
	libiris/cutestuff/legacy/servsock.h
	libiris/cutestuff/legacy/srvresolver.h

	libiris/cutestuff/network/bsocket.h
	libiris/cutestuff/network/httpconnect.h
	libiris/cutestuff/network/socks.h
	libiris/cutestuff/network/httppoll.h

	libiris/cutestuff/util/bytestream.h

	libiris/iris/include/xmpp.h
	libiris/iris/include/xmpp_task.h
	libiris/iris/include/xmpp_client.h
	libiris/iris/include/xmpp_clientstream.h
	#libiris/iris/include/xmpp_liverosteritem.h
	libiris/iris/include/xmpp_stream.h

	libiris/iris/jabber/filetransfer.h
	libiris/iris/jabber/s5b.h
	libiris/iris/jabber/xmpp_ibb.h

	libiris/iris/xmpp-core/compress.h
	libiris/iris/xmpp-core/compressionhandler.h
	libiris/iris/xmpp-core/securestream.h

	libiris/iris/xmpp-im/xmpp_tasks.h
	libiris/iris/xmpp-im/xmpp_discoinfotask.h
	libiris/iris/jabber/jinglesession.h
	libiris/iris/jabber/jingletasks.h
	libiris/iris/jabber/jinglecontent.h
	libiris/iris/jabber/jinglesessionmanager.h
)

set (MOC_SOURCES2_IRIS
	libiris/cutestuff/network/bsocket.cpp
	libiris/iris/jabber/s5b.cpp
	libiris/iris/xmpp-core/securestream.cpp
	libiris/iris/xmpp-im/xmpp_features.cpp
)

set (SOURCES
	file-transfer/jabber-file-transfer.cpp
	gui/widgets/jabber-create-account-widget.cpp
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
	system-info.cpp
	cert-util.cpp
	ssl-cert-dlg.cpp
)

set (MOC_SOURCES
	file-transfer/jabber-file-transfer.h
	gui/widgets/jabber-create-account-widget.h
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
	configuration/jabber_protocol.ui)

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
	libiris/iris/
	libiris/iris/include/
	libiris/iris/jabber/
	libiris/iris/xmpp-core/
	libiris/iris/xmpp-im/
	libiris/cutestuff/util/
	libiris/cutestuff/legacy/
	libiris/cutestuff/network/
	libiris/cutestuff/ )

target_link_libraries(jabber_protocol ${QCA2_LIBRARIES} ${IDN_LIBRARIES} ${QT_LIBRARIES} )

install (FILES ${DATA_FILES}
        DESTINATION share/kadu/modules/data/jabber_protocol)
