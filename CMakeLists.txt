project (kadu)

# configuration

option (ENABLE_AUTDOWNLOAD "Use automatic download of modules/icons/emoticons" ON)
option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)

if (NOT BUILD_DESCRIPTION)
	set (BUILD_DESCRIPTION "Compilied sources" CACHE STRING "Build description" FORCE)
endif (NOT BUILD_DESCRIPTION)

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

if (NOT WIN32)

# TODO: Add libexecinfo support
check_include_files ("execinfo.h" HAVE_EXECINFO)
if (HAVE_EXECINFO)
	check_function_exists ("backtrace" HAVE_EXECINFO)
endif (HAVE_EXECINFO)

if (SIG_HANDLING_ENABLED)
	set (SIG_HANDLING_ENABLED 1)
endif (SIG_HANDLING_ENABLED)

endif (NOT WIN32)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR WIN32)
	set (DEBUG 1)
	set (DEBUG_ENABLED 1)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR WIN32)

if (MSVC)
	add_definitions (/D_CRT_SECURE_NO_WARNINGS=1)
endif (MSVC)

if(MINGW)
	# override cmake bug/feature?
	set (CMAKE_SHARED_LIBRARY_PREFIX "")
endif(MINGW)

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
set(QT_USE_QTDBUS 1)
include(${QT_USE_FILE})
include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

pkg_search_module (LIBGADU REQUIRED libgadu>=1.8.0)

execute_process (
	COMMAND grep "#define GG_CONFIG_HAVE_PTHREAD" ${LIBGADU_INCLUDEDIR}/libgadu.h
	RESULT_VARIABLE LIBGADU_PTHEADS
)

if (NOT APPLE AND ${LIBGADU_PTHEADS} GREATER 0)
	message (FATAL_ERROR "libgadu must be compiled with pthreads support")
endif (NOT APPLE AND ${LIBGADU_PTHEADS} GREATER 0)

include_directories (${LIBGADU_INCLUDE_DIRS})
link_directories (${LIBGADU_LIBRARY_DIRS})

# build

configure_file (${CMAKE_SOURCE_DIR}/kadu-config.h.cmake ${CMAKE_BINARY_DIR}/kadu-config.h)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY)

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

add_subdirectory (modules)
add_subdirectory (kadu-core)
add_subdirectory (varia)
add_subdirectory (translations)
add_subdirectory (kthxbye)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be build:")
message (STATUS " * libgadu (version ${LIBGADU_VERSION}) in ${LIBGADU_LIBRARY_DIRS}")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * debug ${DEBUG} (${CMAKE_BUILD_TYPE})")

# install data files
install (FILES AUTHORS HISTORY README THANKS ChangeLog COPYING DESTINATION "share/kadu")
