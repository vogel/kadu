set (network_SRCS
	network-aware-object.cpp
	network-manager.cpp
	network-manager-ntrack.cpp
	network-manager-qt.cpp
)

set (network_MOC_SRCS
	network-manager.h
	network-manager-ntrack.h
	network-manager-qt.h
)

qt4_wrap_cpp (network_MOC_FILES ${network_MOC_SRCS})

add_library (network
	STATIC
	${network_SRCS}
	${network_MOC_FILES}
)

set_target_properties (network PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (network PROPERTIES COMPILE_FLAGS "-DKADULIB")

if (NOT WIN32)
	include ("../../cmake/FindQNtrack.cmake")
	include_directories (${QNTRACK_INCLUDE_DIR})
endif (NOT WIN32)
