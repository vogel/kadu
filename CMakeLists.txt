project (kadu)

# configuration

option (DEBUG "Build the project using debugging code" OFF CMAKE_BUILD_TYPE "DEBUG")
option (ENABLE_AUTDOWNLOAD "Use automatic download of modules/icons/emoticons" ON)
option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

cmake_minimum_required (VERSION 2.6.0 FATAL_ERROR)

include (FindPkgConfig)
include (CheckIncludeFiles)
include (CheckFunctionExists)

find_package (Subversion)
if (Subversion_FOUND)
	execute_process (COMMAND svnversion -n ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE DETAILED_VERSION)
endif (Subversion_FOUND)

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION KADU_VERSION)
string (REPLACE "\n" "" KADU_VERSION ${KADU_VERSION})

# TODO: Add libexecinfo support 
check_include_files ("execinfo.h" HAVE_EXECINFO)
if (HAVE_EXECINFO)
	check_function_exists ("backtrace" HAVE_EXECINFO)
endif (HAVE_EXECINFO)

if (SIG_HANDLING_ENABLED)
	set (SIG_HANDLING_ENABLED 1)
endif (SIG_HANDLING_ENABLED)

if (DEBUG)
	add_definitions (-g -ggdb -O0)
	set (DEBUG 1)
	set (DEBUG_ENABLED 1)
endif (DEBUG)

set (DATADIR ${CMAKE_INSTALL_PREFIX}/share)
set (LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set (BINDIR ${CMAKE_INSTALL_PREFIX}/bin)

# macros

macro (kadu_configuration)
	install (FILES ${ARGN}
		DESTINATION "share/kadu/configuration")
endmacro (kadu_configuration)

macro (kadu_module_configuration)
	install (FILES ${ARGN}
		DESTINATION "share/kadu/modules/configuration")
endmacro (kadu_module_configuration)

macro (kadu_module_desc)
	install (FILES ${ARGN}
		DESTINATION "share/kadu/modules")
endmacro (kadu_module_desc)

# libraries

set (QT_MIN_VERSION "4.4.0")
find_package (Qt4 REQUIRED)
set(QT_USE_QTXML 1)
set(QT_USE_QNETWORK 1)
set(QT_USE_QT3SUPPORT 1)
set(QT_USE_QTWEBKIT 1)
include(${QT_USE_FILE})
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

pkg_search_module (LIBGADU REQUIRED libgadu>=1.8.0)
include_directories (${LIBGADU_INCLUDE_DIRS})
link_directories (${LIBGADU_LIBRARY_DIRS})

# build

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/kadu-config.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/kadu-config.h)

add_subdirectory (modules)
add_subdirectory (kadu-core)
add_subdirectory (varia)
add_subdirectory (translations)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be build:")
message (STATUS " * libgadu (version ${LIBGADU_VERSION}) in ${LIBGADU_LIBRARY_DIRS}")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * debug ${DEBUG} (${CMAKE_BUILD_TYPE})")

# install data files
install (FILES AUTHORS HISTORY README THANKS ChangeLog COPYING DESTINATION "share/kadu")
