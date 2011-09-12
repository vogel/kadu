project (indicator_docking)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)
include (FindPkgConfig)

set (SOURCES
	indicator_docking.cpp
	indicator_docking-plugin.cpp
)

set (MOC_SOURCES
	indicator_docking.h
	indicator_docking-plugin.h
)

if (NOT WITH_LIBINDICATE_QT)
	message (SEND_ERROR "You need to compile core with -DWITH_LIBINDICATE_QT=ON. Aborting. If you are compiling outside of Kadu core sources, make sure it is compiled with libindicate-qt support and redefine the required macro here.")
endif (NOT WITH_LIBINDICATE_QT)

pkg_search_module (INDICATEQT REQUIRED indicate-qt)

link_directories (${INDICATEQT_LIBRARY_DIRS})
add_definitions (${INDICATEQT_CFLAGS_OTHER})
include_directories (${INDICATEQT_INCLUDE_DIRS})

kadu_plugin (indicator_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${INDICATEQT_LIBRARIES}
	PLUGIN_DEPENDENCIES docking
)

if (NOT INDICATORS_PARENT_DIR)
	if (${CMAKE_INSTALL_PREFIX} MATCHES "/usr/*")
		set (INDICATORS_PARENT_DIR "/usr/share")
	else (${CMAKE_INSTALL_PREFIX} MATCHES "/usr/*")
		if (NOT "$ENV{XDG_CONFIG_HOME}" STREQUAL "")
			set (INDICATORS_PARENT_DIR "$ENV{XDG_CONFIG_HOME}")
		else (NOT "$ENV{XDG_CONFIG_HOME}" STREQUAL "")
			if (NOT "$ENV{HOME}" STREQUAL "")
				set (INDICATORS_PARENT_DIR "$ENV{HOME}/.config")
			else (NOT "$ENV{HOME}" STREQUAL "")
				set (INDICATORS_PARENT_DIR "${CMAKE_INSTALL_PREFIX}/share")
			endif (NOT "$ENV{HOME}" STREQUAL "")
		endif (NOT "$ENV{XDG_CONFIG_HOME}" STREQUAL "")
	endif (${CMAKE_INSTALL_PREFIX} MATCHES "/usr/*")

	message (STATUS "INDICATORS_PARENT_DIR not specified. Using default: ${INDICATORS_PARENT_DIR}")
endif (NOT INDICATORS_PARENT_DIR)

# Pin Kadu to Messaging Menu
file (WRITE ${CMAKE_BINARY_DIR}/kadu "${KADU_DESKTOP_FILE_DIR}/${KADU_DESKTOP_FILE_NAME}\n") 
install (FILES ${CMAKE_BINARY_DIR}/kadu DESTINATION ${INDICATORS_PARENT_DIR}/indicators/messages/applications/)
