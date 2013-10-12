project (indicator_docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	indicator_docking.cpp
	indicator_docking-plugin.cpp
)

set (MOC_SOURCES
	indicator_docking.h
	indicator_docking-plugin.h
)

if (NOT KADU_WITH_LIBINDICATE_QT)
	message (FATAL_ERROR "You need to compile core with libindicate support (-DWITH_LIBINDICATE_QT=ON)")
endif ()

find_package (PkgConfig)
pkg_search_module (LIBINDICATE_QT REQUIRED indicate-qt)

link_directories (${LIBINDICATE_QT_LIBRARY_DIRS})
add_definitions (${LIBINDICATE_QT_CFLAGS_OTHER})
include_directories (${LIBINDICATE_QT_INCLUDE_DIRS})

kadu_plugin (indicator_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${LIBINDICATE_QT_LIBRARIES}
	PLUGIN_DEPENDENCIES docking
)

if (NOT INDICATORS_DIR)
	set (INDICATORS_DIR "${CMAKE_INSTALL_FULL_DATADIR}/indicators/messages/applications")
	message (STATUS "INDICATORS_DIR not specified. Using default: ${INDICATORS_DIR}")
endif ()

# Pin Kadu to Messaging Menu
get_filename_component (desktop_file_path_absolute "${KADU_DESKTOP_FILE_DIR}/${KADU_DESKTOP_FILE_NAME}" ABSOLUTE)
file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/kadu "${desktop_file_path_absolute}\n")
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/kadu DESTINATION "${INDICATORS_DIR}")
