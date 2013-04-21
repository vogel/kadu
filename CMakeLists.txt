project (Kadu)

# Global CMake variables

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
list (INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_CURRENT_BINARY_DIR}")

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

if (WIN32)
	string (REGEX REPLACE "([^\\.]+)\\.([^\\.]+)\\.([^\\.-]+)[\\.-].*" "\\1, \\2, \\3, 0" KADU_NUMERIC_VERSION ${KADU_VERSION})
	if (KADU_VERSION MATCHES ".*-.*")
		set (KADU_PRERELEASE 1)
	endif ()
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

set (path_vars
	INSTALL_SDK_DIR INSTALL_BIN_DIR INSTALL_LIB_DIR INSTALL_DATA_DIR INSTALL_PLUGINS_DATA_DIR
	INSTALL_PLUGINS_LIB_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR DESKTOP_FILE_DIR
)

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

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			PATH_VARS ${path_vars} INCLUDE_DIR SDK_DIR
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	endif ()
endmacro ()

# For the build tree.
kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/KaduConfig.cmake" BUILD_TREE)
# For the installation tree.
kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake" INSTALL_TREE)

string (REGEX REPLACE "-.*" "" simple_version ${KADU_VERSION})
write_basic_package_version_file ("${CMAKE_CURRENT_BINARY_DIR}/KaduConfigVersion.cmake"
	VERSION ${simple_version} COMPATIBILITY AnyNewerVersion
)

# Include the generated file

set (KADU_INSTALLS_SDK TRUE)
find_package (Kadu REQUIRED)

option (BUILD_TESTING "Build the test suite." "${ENABLE_DEVELOPER_BUILD}")

if (NOT WIN32)
	include (CheckIncludeFiles)

	# TODO: Add libexecinfo support
	check_include_files ("execinfo.h" HAVE_EXECINFO)
	if (HAVE_EXECINFO)
		include (CheckFunctionExists)

		check_function_exists ("backtrace" HAVE_EXECINFO)
	endif ()

	if (SIG_HANDLING_ENABLED)
		set (SIG_HANDLING_ENABLED 1)
	endif ()
endif ()

include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

configure_file (${CMAKE_SOURCE_DIR}/kadu-config.h.in ${CMAKE_BINARY_DIR}/kadu-config.h)

if (INSTALL_SDK)
	install (FILES
		"${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/KaduConfigVersion.cmake"
		DESTINATION "${KADU_INSTALL_CMAKE_DIR}"
	)
	install (FILES ${CMAKE_BINARY_DIR}/kadu-config.h DESTINATION ${KADU_INSTALL_SDK_DIR})
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)

add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# kadu-core sets WITH_LIBINDICATE_QT option which is used by Plugins.cmake
add_subdirectory (kadu-core)
include (Plugins.cmake)
add_subdirectory (plugins)
add_subdirectory (varia)
add_subdirectory (translations)
add_subdirectory (kthxbye)
if (BUILD_TESTING)
	enable_testing ()
	add_subdirectory (tests)
endif ()

message (STATUS "Kadu (version: ${KADU_VERSION}) will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")

install (FILES AUTHORS AUTHORS.html HISTORY README THANKS ChangeLog ChangeLog.OLD-PL DESTINATION "${KADU_INSTALL_DATA_DIR}")
# For Win32 we use GPL3, and this COPYING is GPL2
if (NOT WIN32)
	install (FILES COPYING DESTINATION "${KADU_INSTALL_DATA_DIR}")
endif ()
