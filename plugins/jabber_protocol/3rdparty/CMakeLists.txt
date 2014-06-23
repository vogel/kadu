set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOMOC_RELAXED_MODE ON)

set (SOURCES_IRIS
	libiris/src/xmpp/base/randomnumbergenerator.cpp
	libiris/src/xmpp/jid/jid.cpp
	libiris/src/irisnet/noncore/cutestuff/httppoll.cpp
	libiris/src/irisnet/noncore/cutestuff/socks.cpp
	libiris/src/irisnet/noncore/cutestuff/bytestream.cpp
	libiris/src/irisnet/noncore/cutestuff/bsocket.cpp
	libiris/src/irisnet/noncore/cutestuff/httpconnect.cpp
	libiris/src/irisnet/noncore/cutestuff/networkaccessmanager.cpp
	libiris/src/irisnet/noncore/cutestuff/httpstream.cpp
	libiris/src/xmpp/xmpp-im/xmpp_discoitem.cpp
	libiris/src/xmpp/xmpp-im/xmpp_bytestream.cpp
	libiris/src/xmpp/xmpp-im/xmpp_bitsofbinary.cpp
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
	libiris/src/xmpp/sasl/scramsha1message.cpp
	libiris/src/xmpp/sasl/scramsha1response.cpp
	libiris/src/xmpp/sasl/scramsha1signature.cpp
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
	libiris/src/irisnet/corelib/objectsession.cpp
	libiris/src/irisnet/corelib/irisnetglobal.cpp

	libiris/src/jdns/src/jdns/jdns.c
	libiris/src/jdns/src/jdns/jdns_mdnsd.c
	libiris/src/jdns/src/jdns/jdns_packet.c
	libiris/src/jdns/src/jdns/jdns_sys.c
	libiris/src/jdns/src/jdns/jdns_util.c
	libiris/src/jdns/src/qjdns/qjdns.cpp
	libiris/src/jdns/src/qjdns/qjdnsshared.cpp
	libiris/src/jdns/src/qjdns/qjdns_sock.cpp

	libiris/src/irisnet/noncore/cutestuff/bytestream.h
	libiris/src/irisnet/noncore/cutestuff/socks.h
	libiris/src/irisnet/noncore/cutestuff/httpconnect.h
	libiris/src/irisnet/noncore/cutestuff/bsocket.h
	libiris/src/irisnet/noncore/cutestuff/httppoll.h
	libiris/src/irisnet/noncore/cutestuff/networkaccessmanager.h
	libiris/src/irisnet/noncore/cutestuff/httpstream.h
	libiris/src/xmpp/xmpp-im/xmpp_bytestream.h
	libiris/src/xmpp/xmpp-im/xmpp_bitsofbinary.h
	libiris/src/xmpp/xmpp-im/xmpp_tasks.h
	libiris/src/xmpp/xmpp-im/xmpp_discoinfotask.h
	libiris/src/xmpp/xmpp-im/xmpp_ibb.h
	libiris/src/xmpp/xmpp-im/xmpp_client.h
	libiris/src/xmpp/xmpp-im/s5b.h
	libiris/src/xmpp/xmpp-im/xmpp_task.h
	libiris/src/xmpp/xmpp-im/filetransfer.h
	libiris/src/xmpp/sasl/digestmd5proplist.h
	libiris/src/xmpp/sasl/digestmd5response.h
	libiris/src/xmpp/sasl/plainmessage.h
	libiris/src/xmpp/sasl/scramsha1message.h
	libiris/src/xmpp/sasl/scramsha1response.h
	libiris/src/xmpp/sasl/scramsha1signature.h
	libiris/src/xmpp/zlib/zlibdecompressor.h
	libiris/src/xmpp/zlib/zlibcompressor.h
	libiris/src/xmpp/xmpp-core/securestream.h
	libiris/src/xmpp/xmpp-core/compressionhandler.h
	libiris/src/xmpp/xmpp-core/xmpp.h
	libiris/src/xmpp/xmpp-core/xmpp_clientstream.h
	libiris/src/xmpp/xmpp-core/xmpp_stream.h

	libiris/src/jdns/include/jdns/jdns_export.h
	libiris/src/jdns/include/jdns/jdns.h
	libiris/src/jdns/include/jdns/qjdns.h
	libiris/src/jdns/include/jdns/qjdnsshared.h
	libiris/src/jdns/src/jdns/jdns_mdnsd.h
	libiris/src/jdns/src/jdns/jdns_packet.h
	libiris/src/jdns/src/jdns/jdns_p.h
	libiris/src/jdns/src/qjdns/qjdns_p.h
	libiris/src/jdns/src/qjdns/qjdnsshared_p.h
	libiris/src/jdns/src/qjdns/qjdns_sock.h

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
	libiris/src/irisnet/corelib/netavailability.h
	libiris/src/irisnet/corelib/netinterface.h
	libiris/src/irisnet/corelib/netnames.h
	libiris/src/irisnet/corelib/objectsession.h
)

if (WIN32)
	list (APPEND SOURCES_IRIS libiris/src/irisnet/corelib/netinterface_win.cpp)
else ()
	list (APPEND SOURCES_IRIS libiris/src/irisnet/corelib/netinterface_unix.cpp)
endif ()

include_directories (
	${IDN_INCLUDE_DIRS}
	${QCA2_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}

	${CMAKE_CURRENT_SOURCE_DIR}/libiris/include/iris
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/corelib
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/jdns/include/jdns
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/xmpp-core
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src
)

if (WIN32)
	add_definitions (-DJDNS_STATIC)
endif ()

add_library (3rdparty STATIC ${SOURCES_IRIS})
kadu_set_flags (3rdparty)
set_property (TARGET 3rdparty PROPERTY POSITION_INDEPENDENT_CODE TRUE)

if (NOT MSVC)
	set_property (TARGET 3rdparty APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-error -Wno-conversion -Wno-unused-parameter -Wno-unused-function -Wno-cast-align")
endif ()
