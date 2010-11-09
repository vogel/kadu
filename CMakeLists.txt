project (kadu)

# minimal required versions
set (QT_MIN_VERSION "4.5.0")
set (CMAKE_MIN_VERSION "2.6.0")

# configuration
option (ENABLE_AUTODOWNLOAD "Use automatic download of modules/icons/emoticons" ON)
option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)
option (USE_X11_XFIXES "Use X11 Xfixes extension (Desktop Linux)" ON)

if (NOT BUILD_DESCRIPTION)
	set (BUILD_DESCRIPTION "Compiled from sources" CACHE STRING "Build description" FORCE)
endif (NOT BUILD_DESCRIPTION)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

string (REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string (REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# build version string if cmake < 2.6.3
if (NOT DEFINED CMAKE_VERSION)
	set (CMAKE_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
endif (NOT DEFINED CMAKE_VERSION)

cmake_minimum_required (VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)

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
	string (REGEX REPLACE "(.+)\\.(.+)\\.(.+)[.-].*" "\\1, \\2, \\3" NUMERIC_VERSION ${KADU_VERSION})
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

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR WIN32)
	add_definitions (-DDEBUG_ENABLED)

	set (DEBUG 1)
	set (DEBUG_ENABLED 1)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR WIN32)

if (MSVC)
	add_definitions (/D_CRT_SECURE_NO_WARNINGS=1)
endif (MSVC)

if (MINGW)
	# override cmake bug/feature?
	set (CMAKE_SHARED_LIBRARY_PREFIX "")
endif (MINGW)

# warnings
if (UNIX AND NOT APPLE)
	set (CMAKE_C_FLAGS "-W -Wall -Werror -fno-strict-aliasing ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-W -Wall -Werror -fno-strict-aliasing ${CMAKE_CXX_FLAGS}")
elseif (APPLE)
	set (CMAKE_C_FLAGS "-W -Wall -fno-strict-aliasing ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-W -Wall -fno-strict-aliasing ${CMAKE_CXX_FLAGS}")
elseif (MINGW)
	set (CMAKE_C_FLAGS "-ffast-math -mtune=i686 -fno-strict-aliasing")
	set (CMAKE_CXX_FLAGS "-ffast-math -mtune=i686 -fno-strict-aliasing")
endif (UNIX AND NOT APPLE)

if (NOT KADU_DATADIR)
	set (KADU_DATADIR ${CMAKE_INSTALL_PREFIX}/share)
endif (NOT KADU_DATADIR)
if (NOT KADU_LIBDIR)
	set (KADU_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif (NOT KADU_LIBDIR)
if (NOT KADU_BINDIR)
	set (KADU_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
endif (NOT KADU_BINDIR)

if (WIN32)
	set (KADU_DATA_DIR ".")
	set (KADU_MODULES_LIBDIR "modules")
	set (KADU_BIN_DIR ".")
else (WIN32)
	set (KADU_DATA_DIR "share/kadu")
	set (KADU_MODULES_LIBDIR lib${LIB_SUFFIX}/kadu/modules)
	set (KADU_BIN_DIR "bin")
endif (WIN32)

set (KADU_MODULES_DIR ${KADU_DATA_DIR}/modules)

# macros
macro (kadu_configuration)
	install (FILES ${ARGN}
		DESTINATION ${KADU_DATA_DIR}/configuration
	)
endmacro (kadu_configuration)

macro (kadu_module_configuration)
	install (FILES ${ARGN}
		DESTINATION ${KADU_MODULES_DIR}/configuration
	)
endmacro (kadu_module_configuration)

macro (kadu_module_desc MODULE_DESC)
	install (FILES ${ARGN}
		DESTINATION ${KADU_MODULES_DIR}
	)
endmacro (kadu_module_desc)

macro (kadu_module)
	set (VARIABLE_NAME "MODULE_NAME")

	set (MODULE_NAME "")
	set (MODULE_SOURCES "")
	set (MODULE_MOC_SOURCES "")
	set (MODULE_TRANSLATION_SOURCES "")
	set (MODULE_CONFIGURATION_FILES "")
	set (MODULE_DATA_FILES "")
	set (MODULE_DATA_DIRECTORY "")
	set (MODULE_BUILDDEF "")
	set (MODULE_DEPENDENCIES "")
	set (MODULE_LIBRARIES "")

	include_directories (".")

	foreach (ARG ${ARGV})
		if ("${VARIABLE_NAME}" STREQUAL "MODULE_NAME")
			set (MODULE_NAME ${ARG})
			set (VARIABLE_NAME "")
		elseif (ARG MATCHES "(MODULE_SOURCES|MODULE_MOC_SOURCES|MODULE_TRANSLATION_SOURCES|MODULE_CONFIGURATION_FILES|MODULE_DATA_FILES|MODULE_DATA_DIRECTORY|MODULE_BUILDDEF|MODULE_DEPENDENCIES|MODULE_LIBRARIES)")
			set (VARIABLE_NAME ${ARG})
		elseif (VARIABLE_NAME STREQUAL "")
			message (FATAL_ERROR "Invalid invocation of kadu_module macro")
		else ("${VARIABLE_NAME}" STREQUAL "MODULE_NAME")
			set (${VARIABLE_NAME} "${${VARIABLE_NAME}};${ARG}")
		endif ("${VARIABLE_NAME}" STREQUAL "MODULE_NAME")
	endforeach (ARG)

	if (WIN32)
		include_directories (${CMAKE_SOURCE_DIR}/modules)

		# wygeneruj plik z wersja modulu
		if (${${MODULE_NAME}} STREQUAL SHARED)
			set (MODULE_SOURCES ${MODULE_SOURCES} ${MODULE_NAME}.rc)
			add_custom_command (OUTPUT ${MODULE_NAME}.rc
				COMMAND ${CMAKE_SOURCE_DIR}/modules/modver.bat ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}.desc ${MODULE_NAME}.rc
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
				DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}.desc
				COMMENT "Building RC source ${MODULE_NAME}.rc"
			)
		endif (${${MODULE_NAME}} STREQUAL SHARED)

		# naglowki do SDK
		install (DIRECTORY "." DESTINATION ${KADU_DATA_DIR}/kadu-sdk/include/kadu-modules/${MODULE_NAME} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
	endif (WIN32)

	install (FILES ${MODULE_NAME}.desc
		DESTINATION ${KADU_MODULES_DIR}
	)

	if (MODULE_MOC_SOURCES)
		qt_wrap_cpp (${MODULE_NAME} MODULE_MOC_FILES ${MODULE_MOC_SOURCES})
	endif (MODULE_MOC_SOURCES)

	if (MODULE_TRANSLATION_SOURCES)
		qt4_add_translation (MODULE_TRANSLATION_FILES ${MODULE_TRANSLATION_SOURCES})
	endif (MODULE_TRANSLATION_SOURCES)

	kadu_module_configuration (${MODULE_CONFIGURATION_FILES})

	install (FILES ${MODULE_TRANSLATION_FILES}
		DESTINATION ${KADU_MODULES_DIR}/translations
	)

	if (NOT "${MODULE_DATA_FILES}" STREQUAL "")
		install (FILES ${MODULE_DATA_FILES}
			DESTINATION ${KADU_MODULES_DIR}/data/${MODULE_NAME}
		)
	endif (NOT "${MODULE_DATA_FILES}" STREQUAL "")

	if (NOT "${MODULE_DATA_DIRECTORY}" STREQUAL "")
		install (DIRECTORY ${MODULE_DATA_DIRECTORY}
			DESTINATION ${KADU_MODULES_DIR}/data/${MODULE_NAME}
		)
	endif (NOT "${MODULE_DATA_DIRECTORY}" STREQUAL "")

	if (COMMAND cmake_policy)
		cmake_policy (SET CMP0003 NEW)
	endif (COMMAND cmake_policy)

	add_library (${MODULE_NAME} ${${MODULE_NAME}} ${MODULE_SOURCES} ${MODULE_MOC_FILES})
	add_custom_target (${MODULE_NAME}-translations DEPENDS ${MODULE_TRANSLATION_FILES})

	add_dependencies (${MODULE_NAME} ${MODULE_NAME}-translations)

	if (NOT ${MODULE_BUILDDEF} STREQUAL "")
		set_target_properties (${MODULE_NAME} PROPERTIES COMPILE_DEFINITIONS ${MODULE_BUILDDEF})
	endif (NOT ${MODULE_BUILDDEF} STREQUAL "")

	if (NOT "${MODULE_LIBRARIES}" STREQUAL "")
		target_link_libraries (${MODULE_NAME} ${MODULE_LIBRARIES})
	endif (NOT "${MODULE_LIBRARIES}" STREQUAL "")

	if (${${MODULE_NAME}} STREQUAL STATIC)
		set_target_properties (${MODULE_NAME} PROPERTIES COMPILE_FLAGS "-DKADULIB")
	else (${${MODULE_NAME}} STREQUAL STATIC)
		if (WIN32)
			target_link_libraries (${MODULE_NAME} kadu_core ${MODULE_DEPENDENCIES} ${QT_LIBRARIES})
			install (TARGETS ${MODULE_NAME} ARCHIVE DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib)
		endif (WIN32)
		if (APPLE)
			set_target_properties (${MODULE_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
		endif (APPLE)
		install (TARGETS ${MODULE_NAME} RUNTIME DESTINATION ${KADU_MODULES_LIBDIR} LIBRARY DESTINATION ${KADU_MODULES_LIBDIR})
	endif (${${MODULE_NAME}} STREQUAL STATIC)
endmacro (kadu_module)

# libraries
find_package (Qt4 REQUIRED)
set (QT_USE_QTXML 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
set (QT_USE_QTSVG 1)
include (${QT_USE_FILE})
include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core ${QT_INCLUDES})

# build
configure_file (${CMAKE_SOURCE_DIR}/kadu-config.h.cmake ${CMAKE_BINARY_DIR}/kadu-config.h)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)

add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

add_subdirectory (modules)
add_subdirectory (kadu-core)
add_subdirectory (varia)
add_subdirectory (translations)
add_subdirectory (kthxbye)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be build:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")

# install data files
install (FILES AUTHORS HISTORY README THANKS ChangeLog ChangeLog.OLD-PL COPYING DESTINATION ${KADU_DATA_DIR})
