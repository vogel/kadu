project (kadu)

# minimal required versions
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

cmake_minimum_required (VERSION 2.8.0 FATAL_ERROR)

if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)

	if (${CMAKE_VERSION} VERSION_GREATER 2.8.0)
		cmake_policy (SET CMP0015 OLD)
	endif (${CMAKE_VERSION} VERSION_GREATER 2.8.0)
endif (COMMAND cmake_policy)

if (NOT KADU_VERSION)
	if (WIN32)
		set (guessed_paths
			"C:/Program Files/Git/bin"
			"C:/Program Files (x86)/Git/bin"
		)
	else (WIN32)
		set (guessed_paths)
	endif (WIN32)

	find_program (GIT_EXECUTABLE git
		PATHS ${guessed_paths}
		DOC "git command line client"
	)
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
	string (REGEX REPLACE "([^\\.]+)\\.([^\\.]+)\\.([^\\.-]+)[\\.-].*" "\\1, \\2, \\3, 0" KADU_NUMERIC_VERSION ${KADU_VERSION})
	set (PACKAGE_VERSION ${KADU_VERSION})
	if (KADU_VERSION MATCHES ".*-.*")
		set (KADU_PRERELEASE 1)
	endif (KADU_VERSION MATCHES ".*-.*")

	# package
	set (CPACK_GENERATOR "ZIP")
	set (CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
	include (CPack)
endif (WIN32)

if (NOT WIN32)
	include (CheckIncludeFiles)

	# TODO: Add libexecinfo support
	check_include_files ("execinfo.h" HAVE_EXECINFO)
	if (HAVE_EXECINFO)
		include (CheckFunctionExists)

		check_function_exists ("backtrace" HAVE_EXECINFO)
	endif (HAVE_EXECINFO)

	if (SIG_HANDLING_ENABLED)
		set (SIG_HANDLING_ENABLED 1)
	endif (SIG_HANDLING_ENABLED)
endif (NOT WIN32)

set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)
include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

configure_file (${CMAKE_SOURCE_DIR}/kadu-config.h.cmake ${CMAKE_BINARY_DIR}/kadu-config.h)

if (INSTALL_SDK)
	install (FILES cmake/FindKadu.cmake DESTINATION ${KADU_CMAKE_MODULES_DIR})
	install (FILES ${CMAKE_BINARY_DIR}/kadu-config.h DESTINATION ${KADU_SDK_DIR})
endif (INSTALL_SDK)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)

add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# kadu-core sets WITH_LIBINDICATE_QT option which is used by Plugins.cmake
add_subdirectory (kadu-core)
include (Plugins.cmake)
add_subdirectory (plugins)
add_subdirectory (varia)
add_subdirectory (translations)
add_subdirectory (kthxbye)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")

install (FILES AUTHORS HISTORY README THANKS ChangeLog ChangeLog.OLD-PL DESTINATION "${KADU_DATADIR}")
# For Win32 we use GPL3, and this COPYING is GPL2
if (NOT WIN32)
	install (FILES COPYING DESTINATION "${KADU_DATADIR}")
endif (NOT WIN32)
