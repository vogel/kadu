project (Kadu)

# Options (some of them)

option (ENABLE_DEVELOPER_BUILD "Turn on some features helpful during development process (has nothing to do with debugging symbols)" OFF)
option (INSTALL_SDK "Install SDK (API headers, CMake modules, MSVC program libraries)" ON)
option (BUILD_TESTING "Build the test suite." "${ENABLE_DEVELOPER_BUILD}")

# Global CMake variables

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
list (INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_CURRENT_BINARY_DIR}")

if (NOT DEFINED CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	if (ENABLE_DEVELOPER_BUILD)
		set (default Debug)
	else ()
		set (default RelWithDebInfo)
	endif ()

	set (CMAKE_BUILD_TYPE "${default}" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# Warnings and other flags

set (DEFINITIONS QT_USE_QSTRINGBUILDER)
set (DEFINITIONS_DEBUG DEBUG_ENABLED DEBUG_OUTPUT_ENABLED)

if (MSVC)
	set (COMMON_COMPILE_FLAGS "/MP /Zc:wchar_t-")
	list (APPEND DEFINITIONS _CRT_SECURE_NO_WARNINGS=1)
else ()
	set (COMMON_COMPILE_FLAGS "-Wall -Wextra -Wundef -Wcast-align -Wpointer-arith -fno-common")
	set (C_FLAGS "-Wwrite-strings")
	set (CXX_FLAGS "-Woverloaded-virtual -Wnon-virtual-dtor -fno-exceptions")
	list (APPEND DEFINITIONS QT_NO_EXCEPTIONS)

	if (ENABLE_DEVELOPER_BUILD)
		# -pipe can speed up the build
		# -ftrapv generates trap on signed integer overflow, which is undefined by C/C++
		# -fno-omit-frame-pointer gives potentially better stack traces at the cost of negligible performance drop
		set (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Werror -pipe -ftrapv -fno-omit-frame-pointer")

		# TODO: uncomment for Clang 3.3
		#if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		#	# run-time checks for undefined or suspicious behavior
		#	set (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fsanitize=undefined")
		#endif ()

		if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "GNU")
			# -z now checks there are no unresolved symbols at executalbe/library load time, instead of that specific symbol load time
			set (LINK_FLAGS "-Wl,-z,now -Wl,--fatal-warnings")
		endif ()
	endif ()
endif ()

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

# Version information

file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" KADU_VERSION LIMIT_COUNT 1)

find_package (Git QUIET)
if (GIT_FOUND)
	execute_process (COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE git_tag
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (NOT (result STREQUAL 0) OR NOT (git_tag STREQUAL KADU_VERSION))
		execute_process (COMMAND "${GIT_EXECUTABLE}" rev-parse --short --verify HEAD
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			RESULT_VARIABLE result
			OUTPUT_VARIABLE git_commit
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		if (result STREQUAL 0)
			set (KADU_VERSION "${KADU_VERSION}-g${git_commit}")
		endif ()
	endif ()
endif ()

string (REGEX REPLACE "-.*" "" simple_version ${KADU_VERSION})

if (WIN32)
	if (NOT KADU_VERSION STREQUAL simple_version)
		set (KADU_PRERELEASE 1)
	endif ()

	# Change "."'s to ","'s.
	string (REPLACE "." ", " KADU_NUMERIC_VERSION ${simple_version})
	# Remove 5-th and further components, if any.
	string (REGEX REPLACE "(^[^,]*,[^,]*,[^,]*,[^,]*).*" "\\1" KADU_NUMERIC_VERSION "${KADU_NUMERIC_VERSION}")
	# Add 4-th component if not present.
	string (REGEX REPLACE "(^[^,]*,[^,]*,[^,]*$)" "\\1, 0" KADU_NUMERIC_VERSION "${KADU_NUMERIC_VERSION}")
	# Add 3-rd and 4-th components if not present.
	string (REGEX REPLACE "(^[^,]*,[^,]*$)" "\\1, 0, 0" KADU_NUMERIC_VERSION "${KADU_NUMERIC_VERSION}")
	# Add 2-nd, 3-rd and 4-th components if not present.
	string (REGEX REPLACE "(^[^,]*,[^,]*$)" "\\1, 0, 0, 0" KADU_NUMERIC_VERSION "${KADU_NUMERIC_VERSION}")
endif ()

# Installation paths.

if (NOT INSTALL_SDK_DIR)
	set (INSTALL_SDK_DIR "" CACHE PATH "SDK installation root")
	set (INSTALL_SDK_DIR "${CMAKE_INSTALL_PREFIX}/sdk")
endif ()
mark_as_advanced (INSTALL_SDK_DIR)

# Override default GNU install dirs on Windows.
if (WIN32)
	set (CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "user executables")
	set (CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "object code libraries")
	set (CMAKE_INSTALL_INCLUDEDIR "${INSTALL_SDK_DIR}/include" CACHE PATH "C header files")
	set (CMAKE_INSTALL_DATAROOTDIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "read-only architecture-independent data root")
endif ()

include (GNUInstallDirs)

set (INSTALL_BIN_DIR "${CMAKE_INSTALL_FULL_BINDIR}")
set (INSTALL_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
if (WIN32)
	set (INSTALL_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}")
else ()
	set (INSTALL_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/kadu")
endif ()

set (INSTALL_PLUGINS_DATA_DIR "${INSTALL_DATA_DIR}/plugins")
if (WIN32)
	set (INSTALL_PLUGINS_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/plugins")
else ()
	set (INSTALL_PLUGINS_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/kadu/plugins")
endif ()

if (WIN32)
	set (INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
else ()
	set (INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}/kadu")
endif ()

if (WIN32)
	set (INSTALL_CMAKE_DIR "${INSTALL_SDK_DIR}/cmake")
else ()
	set (INSTALL_CMAKE_DIR "${INSTALL_LIB_DIR}/cmake/Kadu")
endif ()

file (RELATIVE_PATH KADU_DATADIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${INSTALL_DATA_DIR}")
file (RELATIVE_PATH KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${INSTALL_PLUGINS_LIB_DIR}")

if (UNIX AND NOT APPLE)
	set (DESKTOP_FILE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/applications")
	set (DESKTOP_FILE_NAME kadu.desktop)

	file (RELATIVE_PATH KADU_DESKTOP_FILE_DIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${DESKTOP_FILE_DIR}")
endif ()

set (install_vars
	INSTALL_SDK_DIR INSTALL_BIN_DIR INSTALL_LIB_DIR INSTALL_DATA_DIR INSTALL_PLUGINS_DATA_DIR
	INSTALL_PLUGINS_LIB_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR
)
foreach (install_var ${install_vars})
	if (NOT DEFINED ${install_var})
		message (FATAL_ERROR "Variable ${install_var} does not exist")
	else ()
		if (IS_ABSOLUTE "${${install_var}}")
			string (REPLACE "${CMAKE_INSTALL_PREFIX}/" "" RELATIVE_${install_var} "${${install_var}}")
			if (IS_ABSOLUTE "${RELATIVE_${install_var}}")
				message (FATAL_ERROR "Internal error, RELATIVE_${install_var} is absolute (${RELATIVE_${install_var}})")
			endif ()
		else ()
			set (RELATIVE_${install_var} "${${install_var}}")
		endif()
	endif ()
endforeach ()

set (INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set (path_vars INSTALL_PREFIX)
if (UNIX AND NOT APPLE)
	list (APPEND path_vars DESKTOP_FILE_DIR)
endif ()

# Generate CMake configuration files

include (CMakePackageConfigHelpers)

macro (kadu_configure_package_config_file _in _out _mode)
	if ("${_mode}" STREQUAL BUILD_TREE)
		foreach (path_var ${path_vars})
			set (PACKAGE_${path_var} "${${path_var}}")
		endforeach ()

		set (PACKAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
		set (PACKAGE_SDK_DIR "${CMAKE_SOURCE_DIR}")

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	elseif ("${_mode}" STREQUAL INSTALL_TREE)
		set (INCLUDE_DIR "${INSTALL_INCLUDE_DIR}")
		set (SDK_DIR "${INSTALL_SDK_DIR}")
		list (APPEND path_vars INCLUDE_DIR SDK_DIR)

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			PATH_VARS ${path_vars}
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	endif ()
endmacro ()

# This one is only for usable within the build tree. We want to actually
# set some variables in there from within the subdirectories, so we generated
# the intallation version later.
kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/KaduConfig.cmake" BUILD_TREE)

write_basic_package_version_file ("${CMAKE_CURRENT_BINARY_DIR}/KaduConfigVersion.cmake"
	VERSION ${simple_version} COMPATIBILITY AnyNewerVersion
)

# Include the generated file

find_package (Kadu REQUIRED)

if (NOT WIN32)
	include (CheckIncludeFiles)

	# TODO: Add libexecinfo support
	check_include_files ("execinfo.h" HAVE_EXECINFO)
	if (HAVE_EXECINFO)
		include (CheckFunctionExists)

		check_function_exists ("backtrace" HAVE_EXECINFO)
	endif ()
endif ()

include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

configure_file (${CMAKE_SOURCE_DIR}/kadu-config.h.in ${CMAKE_BINARY_DIR}/kadu-config.h)

add_subdirectory (kadu-core)
add_subdirectory (varia)
add_subdirectory (translations)

include (Plugins.cmake)
add_subdirectory (plugins)

if (BUILD_TESTING)
	enable_testing ()
	add_subdirectory (tests)
endif ()

add_subdirectory (kthxbye)

# Now write KaduConfig for installation.
kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake" INSTALL_TREE)

if (KADU_INSTALL_SDK)
	install (FILES
		"${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/KaduConfigVersion.cmake"
		cmake/KaduMacros.cmake
		DESTINATION "${KADU_INSTALL_CMAKE_DIR}"
	)
	install (FILES ${CMAKE_BINARY_DIR}/kadu-config.h DESTINATION ${KADU_INSTALL_SDK_DIR})

	install (EXPORT KaduTargets DESTINATION "${KADU_INSTALL_CMAKE_DIR}")
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)
add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

message (STATUS "Kadu (version: ${KADU_VERSION}) will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
	message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
endif ()

install (FILES AUTHORS AUTHORS.html HISTORY README THANKS ChangeLog ChangeLog.OLD-PL DESTINATION "${KADU_INSTALL_DATA_DIR}")
# For Win32 we use GPL3, and this COPYING is GPL2
if (NOT WIN32)
	install (FILES COPYING DESTINATION "${KADU_INSTALL_DATA_DIR}")
endif ()
