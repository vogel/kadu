project (kadu)

# configuration

option (DEBUG "Build the project using debugging code" OFF CMAKE_BUILD_TYPE "DEBUG")
option (ENABLE_AUTDOWNLOAD "Use automatic download of modules/icons/emoticons" ON)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

cmake_minimum_required (VERSION 2.4.0)

find_file (KADU_VERSION_FILE VERSION PATHS ${CMAKE_CURRENT_SOURCE_DIR})
exec_program (svnversion
	ARGS -n ${CMAKE_CURRENT_SOURCE_DIR} > ${CMAKE_CURRENT_SOURCE_DIR}/SNAPSHOT)
find_file (KADU_SNAPSHOT_FILE VERSION PATHS ${CMAKE_CURRENT_SOURCE_DIR})

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION KADU_VERSION)
string (REPLACE "\n" "" KADU_VERSION ${KADU_VERSION})

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/SNAPSHOT DETAILED_VERSION)
string (REPLACE "\n" "" DETAILED_VERSION ${DETAILED_VERSION})

if (DEBUG)
	add_definitions (-ggdb -O)
	set (DEBUG 1)
	set (DEBUG_ENABLED 1)
endif (DEBUG)

set (DATADIR ${CMAKE_INSTALL_PREFIX}/share)
set (LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set (BINDIR ${CMAKE_INSTALL_PREFIX}/bin)

# libraries

find_package (Qt4 4.4 REQUIRED)
add_definitions (${QT_DEFINITIONS})
add_definitions (-DQT3_SUPPORT)
include_directories (${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

find_library (LIBGADU NAMES gadu PATHS /usr/local/lib /usr/lib)

# build

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kadu-config.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/kadu-config.h )

add_subdirectory (kadu-core)
add_subdirectory (varia)
add_subdirectory (translations)
add_subdirectory (modules)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be build:")
message (STATUS " * libgadu in ${LIBGADU}")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * debug ${DEBUG} (${CMAKE_BUILD_TYPE})")
