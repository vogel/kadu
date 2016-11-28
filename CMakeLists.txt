project (Kadu)

cmake_minimum_required (VERSION 2.8.11)

include (cmake/options.cmake)

# Global CMake variables

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

if (NOT DEFINED CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	if (ENABLE_DEVELOPER_BUILD)
		set (default Debug)
	else ()
		set (default RelWithDebInfo)
	endif ()

	set (CMAKE_BUILD_TYPE "${default}" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# C++14 support, warnings and other flags

set (DEFINITIONS QT_NO_CAST_TO_ASCII QT_DISABLE_DEPRECATED_BEFORE=0x040900)
list (APPEND DEFINITIONS KADU_EXPORT_TESTS)
set (DEFINITIONS_DEBUG DEBUG_ENABLED DEBUG_OUTPUT_ENABLED)

if (MSVC)
	set (CXX_FLAGS "${CXX_FLAGS} /MP /Zc:wchar_t-")
	list (APPEND DEFINITIONS _CRT_SECURE_NO_WARNINGS=1)
else ()
	set (CXX_FLAGS "${CXX_FLAGS} -Wall -Wextra -Wundef -Wunused -Wuninitialized -Wcast-align -Wpointer-arith -fno-common")
	set (C_FLAGS "-Wwrite-strings")
	set (CXX_FLAGS "-Woverloaded-virtual -Wnon-virtual-dtor ${CXX_FLAGS}")
	set (CMAKE_CXX_STANDARD 14)

	if (NOT WIN32)
		set (CXX_FLAGS "${CXX_FLAGS} -fvisibility=hidden")
		set (CXX_FLAGS "${CXX_FLAGS} -fvisibility-inlines-hidden")
	endif ()

	if (CMAKE_CXX_COMPILER MATCHES "Clang")
		set (CXX_FLAGS "${CXX_FLAGS} -U__GNUC_MINOR__ -D__GNUC_MINOR__=8")
	endif ()

	if (ENABLE_DEVELOPER_BUILD)
		# -pipe can speed up the build
		# -ftrapv generates trap on signed integer overflow, which is undefined by C/C++
		# -fno-omit-frame-pointer gives potentially better stack traces at the cost of negligible performance drop
		set (CXX_FLAGS "${CXX_FLAGS} -Werror -Wno-deprecated -pipe -ftrapv -fno-omit-frame-pointer")

		if (CMAKE_CXX_COMPILER MATCHES "clang")
			set (SANITIZE_OPTIONS "-fsanitize=undefined -fsanitize=integer -fsanitize=undefined-trap -fsanitize=unsigned-integer-overflow")
			set (CXX_FLAGS "${CXX_FLAGS} ${SANITIZE_OPTIONS} -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/sanitize-blacklist.txt")
			set (LINK_FLAGS "${LINK_FLAGS} ${SANITIZE_OPTIONS}")
			# Clang links its sanitizer libraries only to executables,
			# even though they are actually needed all over the code. :-(
			set (ALLOW_UNDEFINED ON)
		endif ()

		if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "GNU")
			# -z now checks there are no unresolved symbols at executalbe/library load time, instead of that specific symbol load time
			set (LINK_FLAGS "${LINK_FLAGS} -Wl,-z,now -Wl,--as-needed")
		endif ()
	endif ()

	if (NOT ALLOW_UNDEFINED AND (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "GNU"))
		set (LINK_FLAGS "${LINK_FLAGS} -Wl,--no-undefined")
	endif ()
endif ()

include (cmake/ccache.cmake)

# Look above, DEBUG_ENABLED is always enabled in Debug configuration.
# Enable it in all configurations when ENABLE_DEVELOPER_BUILD is on.
if (ENABLE_DEVELOPER_BUILD)
	list (APPEND DEFINITIONS DEBUG_ENABLED)
endif ()

# The same notes as DEBUG_ENABLED concern also DEBUG_OUTPUT_ENABLED.
# Additionally, Windows needs DEBUG_OUTPUT_ENABLED because we always
# install kadu_c.exe with console output.
if (ENABLE_DEVELOPER_BUILD OR WIN32)
	list (APPEND DEFINITIONS DEBUG_OUTPUT_ENABLED)
endif ()

include (cmake/version.cmake)

# if ((NOT DEFINED NETWORK_IMPLEMENTATION AND NOT Qt5Core_FOUND) OR "${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
# 	find_package (QNtrack 010)
# endif ()

if (NOT DEFINED NETWORK_IMPLEMENTATION)
#	if (QNTRACK_FOUND)
#		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: ntrack")
#		set (NETWORK_IMPLEMENTATION "ntrack")
#	else ()
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: Qt")
		set (NETWORK_IMPLEMENTATION "Qt")
#	endif ()
endif ()

set (NETWORK_IMPLEMENTATION "${NETWORK_IMPLEMENTATION}" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)

if ("${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
	list (APPEND ADDITIONAL_LIBKADU_LIBRARIES ${QNTRACK_LIBRARIES})
endif ()
# The rest of NETWORK_IMPLEMENTATION stuff is handled in network/CMakeLists.txt.

include (cmake/injeqt.cmake)
include (cmake/tests.cmake)
include (cmake/install.cmake)
include (cmake/config-files.cmake)

if (NOT WIN32)
	include (CheckIncludeFiles)

	# TODO: Add libexecinfo support
	check_include_files ("execinfo.h" HAVE_EXECINFO)
	if (HAVE_EXECINFO)
		include (CheckFunctionExists)

		check_function_exists ("backtrace" HAVE_EXECINFO)
	endif ()
endif ()

include (cmake/include-dirs.cmake)
include (cmake/build-dirs.cmake)

# Now write KaduConfig for installation.
if (UNIX)
	kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
		"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake" INSTALL_TREE)
endif ()

if (KADU_INSTALL_SDK)
	install (FILES
		"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake"
		"${CMAKE_BINARY_DIR}/KaduConfigVersion.cmake"
		cmake/KaduMacros.cmake
		DESTINATION "${KADU_INSTALL_CMAKE_DIR}"
	)

	install (EXPORT KaduTargets DESTINATION "${KADU_INSTALL_CMAKE_DIR}")
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)
add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

configure_file (varia/scripts/nsis_installer.nsi.in "${CMAKE_CURRENT_BINARY_DIR}/nsis_installer.nsi" ESCAPE_QUOTES @ONLY)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
	message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
endif ()

install (FILES AUTHORS AUTHORS.html HISTORY README THANKS ChangeLog ChangeLog.OLD-PL DESTINATION "${KADU_INSTALL_DATA_DIR}")
# For Win32 we use GPL3, and this COPYING is GPL2
if (WIN32)
	install (FILES COPYING.WIN32 DESTINATION "${KADU_INSTALL_DATA_DIR}")
else ()
	install (FILES COPYING.GPL2 COPYING.LGPL2.1 DESTINATION "${KADU_INSTALL_DATA_DIR}")
endif ()
