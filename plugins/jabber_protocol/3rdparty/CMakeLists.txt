# based on qt4_automoc
macro (kadu_jabber_automoc)
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
endmacro (kadu_jabber_automoc)

set (SOURCES_IRIS
	libiris/src/xmpp/base/randomnumbergenerator.cpp
	libiris/src/xmpp/jid/jid.cpp
	libiris/src/irisnet/noncore/cutestuff/httppoll.cpp
	libiris/src/irisnet/noncore/cutestuff/socks.cpp
	libiris/src/irisnet/noncore/cutestuff/bytestream.cpp
	libiris/src/irisnet/noncore/cutestuff/bsocket.cpp
	libiris/src/irisnet/noncore/cutestuff/httpconnect.cpp
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

if (NOT MSVC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif (NOT MSVC)

set (MOC_SOURCES_IRIS
	libiris/src/irisnet/noncore/cutestuff/bytestream.h
	libiris/src/irisnet/noncore/cutestuff/socks.h
	libiris/src/irisnet/noncore/cutestuff/httpconnect.h
	libiris/src/irisnet/noncore/cutestuff/bsocket.h
	libiris/src/irisnet/noncore/cutestuff/httppoll.h
	libiris/src/xmpp/xmpp-im/xmpp_bytestream.h
	libiris/src/xmpp/xmpp-im/xmpp_bitsofbinary.h
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

include_directories (
	${IDN_INCLUDE_DIRS}
	${QCA2_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}

	${CMAKE_CURRENT_SOURCE_DIR}/libiris/include/iris
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/irisnet/corelib
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src/xmpp/xmpp-core/
	${CMAKE_CURRENT_SOURCE_DIR}/libiris/src
)

qt4_wrap_cpp (MOC_SOURCES_FILES ${MOC_SOURCES_IRIS})
kadu_jabber_automoc (${MOC_SOURCES2_IRIS})

add_library (3rdparty STATIC ${SOURCES_IRIS} ${MOC_SOURCES2_IRIS} ${MOC_SOURCES_FILES})
if (NOT WIN32)
	set_target_properties (3rdparty PROPERTIES COMPILE_FLAGS -fPIC)
endif (NOT WIN32)
