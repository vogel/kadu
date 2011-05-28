set (network_SRCS
	network-aware-object.cpp
	network-manager.cpp
)

set (network_MOC_SRCS
	network-manager.h
)

message (STATUS "Using ${NETWORK_IMPLEMENTATION} network implementation")

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

	set (QNtrack_FIND_REQUIRED true)

	include ("../../cmake/FindQNtrack.cmake")
	include_directories (${QNTRACK_INCLUDE_DIR})

	set (network_SRCS ${network_SRCS}
		network-manager-ntrack.cpp
	)

	set (network_MOC_SRCS ${network_MOC_SRCS}
		network-manager-ntrack.h
	)
else (${NETWORK_IMPLEMENTATION} STREQUAL "Qt")
	set (NETWORK_IMPLEMENTATION_INCLUDE network-manager-dummy.h)
	set (NETWORK_IMPLEMENTATION_CLASS_NAME NetworkManagerDummy)

	set (network_SRCS ${network_SRCS}
		network-manager-dummy.cpp
	)

	set (network_MOC_SRCS ${network_MOC_SRCS}
		network-manager-dummy.h
	)
endif (${NETWORK_IMPLEMENTATION} STREQUAL "Qt")

qt4_wrap_cpp (network_MOC_FILES ${network_MOC_SRCS})

add_library (network
	STATIC
	${network_SRCS}
	${network_MOC_FILES}
)

set_target_properties (network PROPERTIES COMPILE_DEFINITIONS KADULIB)

include_directories (${CMAKE_CURRENT_BINARY_DIR})
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/kadu-network-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kadu-network-config.h)
