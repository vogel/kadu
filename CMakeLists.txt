project (kadu)

# minimal required versions
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
option (BUILD_PLUGINS "Switch to Off if you do not want to build plugins" ON)

# configuration
option (INSTALL_CMAKE_MODULE "Install cmake modules" ON)
if (UNIX)
	option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)
	if (UNIX AND NOT APPLE)
		option (USE_X11_XFIXES "Use X11 XFixes extension (Desktop Linux)" ON)
	endif (UNIX AND NOT APPLE)
endif (UNIX)

if (NOT NETWORK_IMPLEMENTATION)
	include ("cmake/FindQNtrack.cmake")
	if (QNTRACK_FOUND)
		set (NETWORK_IMPLEMENTATION "ntrack" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: ntrack")
	else (QNTRACK_FOUND)
		set (NETWORK_IMPLEMENTATION "Qt" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: Qt")
	endif (QNTRACK_FOUND)
endif (NOT NETWORK_IMPLEMENTATION)

if (NOT BUILD_DESCRIPTION)
	set (BUILD_DESCRIPTION "Compiled from sources" CACHE STRING "Build description" FORCE)
endif (NOT BUILD_DESCRIPTION)

# build version string if cmake < 2.6.3
if (NOT DEFINED CMAKE_VERSION)
	set (CMAKE_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
endif (NOT DEFINED CMAKE_VERSION)

cmake_minimum_required (VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)

if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)

	if (${CMAKE_VERSION} VERSION_GREATER 2.8.0)
		cmake_policy (SET CMP0015 OLD)
	endif (${CMAKE_VERSION} VERSION_GREATER 2.8.0)
endif (COMMAND cmake_policy)

include (FindPkgConfig)
include (CheckIncludeFiles)
include (CheckFunctionExists)

if (NOT KADU_VERSION)
	find_program (GIT_EXECUTABLE git DOC "git command line client")
	if (GIT_EXECUTABLE)
		execute_process (COMMAND ${GIT_EXECUTABLE} describe --tags HEAD
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			OUTPUT_VARIABLE KADU_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	endif (GIT_EXECUTABLE)

	if (NOT KADU_VERSION)
		file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION KADU_VERSION)
		string (REPLACE "\n" "" KADU_VERSION ${KADU_VERSION})
	endif (NOT KADU_VERSION)
endif (NOT KADU_VERSION)

if (WIN32)
	# Add more version info to files
	string (REGEX REPLACE "(.+)\\.(.+)\\.(.+)[.-].*" "\\1, \\2, \\3, 0" NUMERIC_VERSION ${KADU_VERSION})
	set (PACKAGE_VERSION ${KADU_VERSION})

	# package
	set (CPACK_GENERATOR "ZIP")
	set (CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
	include (CPack)
endif (WIN32)

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

# macros
macro (kadu_source_subdirectories CURRENT_TARGET)
	foreach (ARG ${ARGN})
		add_subdirectory (${ARG})
		link_directories (${ARG})

		add_dependencies (${CURRENT_TARGET}
			${ARG}
		)
	endforeach (ARG)

if (APPLE)
	target_link_libraries (${CURRENT_TARGET} -Wl,-all_load ${ARGN})
else (APPLE)
	target_link_libraries (${CURRENT_TARGET} -Wl,--start-group -Wl,--whole-archive ${ARGN} -Wl,--no-whole-archive -Wl,--end-group)
endif (APPLE)
endmacro (kadu_source_subdirectories)

include (cmake/FindKadu.cmake)
include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

if (INSTALL_CMAKE_MODULE)
	install (FILES cmake/FindKadu.cmake DESTINATION ${KADU_CMAKE_MODULES_DIR})
	install (PROGRAMS translations/plugintsupdate.sh DESTINATION ${KADU_PLUGINS_DIR}/sdk)
	install (FILES translations/configuration-ui.xsl DESTINATION ${KADU_PLUGINS_DIR}/sdk)
endif (INSTALL_CMAKE_MODULE)

# build
configure_file (${CMAKE_SOURCE_DIR}/kadu-config.h.cmake ${CMAKE_BINARY_DIR}/kadu-config.h)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)

add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

include (Plugins.cmake)

add_subdirectory (kadu-core)
add_subdirectory (plugins)
add_subdirectory (varia)
add_subdirectory (translations)
add_subdirectory (kthxbye)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")

# install data files
install (FILES AUTHORS HISTORY README THANKS ChangeLog ChangeLog.OLD-PL COPYING DESTINATION ${KADU_DATA_DIR})
