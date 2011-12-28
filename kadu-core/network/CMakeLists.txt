set (network_SRCS
	proxy/model/network-proxy-model.cpp
	proxy/model/network-proxy-proxy-model.cpp

	proxy/network-proxy.cpp
	proxy/network-proxy-manager.cpp
	proxy/network-proxy-shared.cpp

	network-aware-object.cpp
	network-manager.cpp
)

set (network_MOC_SRCS
	proxy/model/network-proxy-model.h
	proxy/model/network-proxy-proxy-model.h

	proxy/network-proxy-manager.h
	proxy/network-proxy-shared.h

	network-manager.h
)

if (${NETWORK_IMPLEMENTATION} STREQUAL "Qt")
	set (NETWORK_IMPLEMENTATION_INCLUDE network-manager-qt.h)
	set (NETWORK_IMPLEMENTATION_CLASS_NAME NetworkManagerQt)

	set (network_SRCS ${network_SRCS}
		network-manager-qt.cpp
	)

	set (network_MOC_SRCS ${network_MOC_SRCS}
		network-manager-qt.h
	)
elseif (${NETWORK_IMPLEMENTATION} STREQUAL "ntrack")
	set (NETWORK_IMPLEMENTATION_INCLUDE network-manager-ntrack.h)
	set (NETWORK_IMPLEMENTATION_CLASS_NAME NetworkManagerNTrack)

	include_directories (${QNTRACK_INCLUDE_DIR})

	set (network_SRCS ${network_SRCS}
		network-manager-ntrack.cpp
	)

	set (network_MOC_SRCS ${network_MOC_SRCS}
		network-manager-ntrack.h
	)
else (${NETWORK_IMPLEMENTATION} STREQUAL "Qt")
	if (NOT ${NETWORK_IMPLEMENTATION} STREQUAL "dummy")
		message (STATUS "Unknown ${NETWORK_IMPLEMENTATION} network implementation")
		set (NETWORK_IMPLEMENTATION "dummy")
	endif (NOT ${NETWORK_IMPLEMENTATION} STREQUAL "dummy")

	set (NETWORK_IMPLEMENTATION_INCLUDE network-manager-dummy.h)
	set (NETWORK_IMPLEMENTATION_CLASS_NAME NetworkManagerDummy)

	set (network_SRCS ${network_SRCS}
		network-manager-dummy.cpp
	)

	set (network_MOC_SRCS ${network_MOC_SRCS}
		network-manager-dummy.h
	)
endif (${NETWORK_IMPLEMENTATION} STREQUAL "Qt")

message (STATUS "Using ${NETWORK_IMPLEMENTATION} network implementation")

include_directories (${CMAKE_CURRENT_BINARY_DIR})
configure_file (kadu-network-config.h.cmake kadu-network-config.h)

kadu_subdirectory (network "${network_SRCS}" "${network_MOC_SRCS}" "${CMAKE_CURRENT_BINARY_DIR}")
