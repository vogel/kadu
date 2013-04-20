project (kadu)

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
list (INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_CURRENT_BINARY_DIR}")

set (KADU_INSTALLS_SDK TRUE)

# For the build tree.
configure_file (cmake/KaduConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/KaduConfig.cmake" @ONLY)
# For the installation tree.
configure_file (cmake/KaduConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake" @ONLY)

find_package (Kadu REQUIRED)

option (BUILD_TESTING "Build the test suite." "${ENABLE_DEVELOPER_BUILD}")

if (WIN32)
	set (guessed_paths
		"C:/Program Files/Git/bin"
		"C:/Program Files (x86)/Git/bin"
	)
else ()
	set (guessed_paths)
endif ()

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
endif ()

if (NOT KADU_VERSION)
	file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" KADU_VERSION LIMIT_COUNT 1)
endif ()

if (WIN32)
	# Add more version info to files
	string (REGEX REPLACE "([^\\.]+)\\.([^\\.]+)\\.([^\\.-]+)[\\.-].*" "\\1, \\2, \\3, 0" KADU_NUMERIC_VERSION ${KADU_VERSION})
	set (PACKAGE_VERSION ${KADU_VERSION})
	if (KADU_VERSION MATCHES ".*-.*")
		set (KADU_PRERELEASE 1)
	endif ()

	# package
	set (CPACK_GENERATOR "ZIP")
	set (CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
	include (CPack)
endif ()

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
		"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake"
		DESTINATION "${KADU_INSTALL_CMAKE_DIR}"
	)
	install (FILES ${CMAKE_BINARY_DIR}/kadu-config.h DESTINATION ${KADU_SDK_DIR})
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

install (FILES AUTHORS AUTHORS.html HISTORY README THANKS ChangeLog ChangeLog.OLD-PL DESTINATION "${KADU_DATADIR}")
# For Win32 we use GPL3, and this COPYING is GPL2
if (NOT WIN32)
	install (FILES COPYING DESTINATION "${KADU_DATADIR}")
endif ()
