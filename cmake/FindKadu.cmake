# Try to find Kadu development headers
# Once done this will define
#
#  KADU_FOUND - system has Kadu headers
#  KADU_INCLUDE_DIR - the Kadu include directory
#
# Based on FindQutIM by Ruslan Nigmatullin, <euroelessar@gmail.com>
# Copyright (c) 2009, Ruslan Nigmatullin, <euroelessar@gmail.com>
# Copyrignt (c) 2011, Rafał 'Vogel' Malinowski <vogel@kadu.im>

# libraries
set (QT_USE_QTXML 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
if (UNIX AND NOT APPLE)
	set (QT_USE_QTDBUS 1)
endif (UNIX AND NOT APPLE)
find_package (Qt4 4.7.0 REQUIRED)
include (${QT_USE_FILE})

if (KADU_INCLUDE_DIR)
	# Already in cache, be silent
	set (KADU_FIND_QUIETLY TRUE)
endif (KADU_INCLUDE_DIR)

if (NOT KADU_DO_NOT_FIND)
	find_path (KADU_INCLUDE_DIR
		kadu-core/kadu-application.h
		PATHS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../.. ${KADU_SEARCH_DIRS}
		PATH_SUFFIXES include/kadu sdk/include include
	)

	find_path (KADU_CURRENT_SDK_DIR
		translations/plugintsupdate.sh
		PATHS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../.. ${KADU_SEARCH_DIRS}
		PATH_SUFFIXES sdk
	)

	if (KADU_INCLUDE_DIR)
		if (NOT KADU_FIND_QUIETLY)
			message (STATUS "Found Kadu headers: ${KADU_INCLUDE_DIR}/kadu")
		endif (NOT KADU_FIND_QUIETLY)
	else (KADU_INCLUDE_DIR)
		if (KADU_FIND_REQUIRED)
			message (FATAL_ERROR "Could not find Kadu development headers")
		else (KADU_FIND_REQUIRED)
			message (STATUS "Could not find Kadu development headers")
		endif (KADU_FIND_REQUIRED)
    endif (KADU_INCLUDE_DIR)
endif (NOT KADU_DO_NOT_FIND)

option (ENABLE_DEVELOPER_BUILD "Turn on some features helpful during development process (has nothing to do with debugging symbols)" OFF)
if (KADU_INSTALLS_SDK OR WIN32)
	option (INSTALL_SDK "Install SDK (API headers, CMake modules, MSVC program libraries)" ON)
endif (KADU_INSTALLS_SDK OR WIN32)

if (NOT MSVC AND NOT XCODE_VERSION AND NOT CMAKE_BUILD_TYPE)
	if (ENABLE_DEVELOPER_BUILD)
		set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
	else (ENABLE_DEVELOPER_BUILD)
		set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
	endif (ENABLE_DEVELOPER_BUILD)
endif (NOT MSVC AND NOT XCODE_VERSION AND NOT CMAKE_BUILD_TYPE)

# never use -O3 on GCC unless explicitly set by the user
if (CMAKE_COMPILER_IS_GNUCXX)
	string (REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
	string (REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif (CMAKE_COMPILER_IS_GNUCXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_DEVELOPER_BUILD OR WIN32)
	set (DEBUG_ENABLED 1)
	add_definitions (-DDEBUG_ENABLED)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_DEVELOPER_BUILD OR WIN32)

if (MINGW AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (-D_DEBUG)
endif (MINGW AND CMAKE_BUILD_TYPE STREQUAL "Debug")

# Qt 4.7
add_definitions (-DQT_USE_FAST_CONCATENATION)
add_definitions (-DQT_USE_FAST_OPERATOR_PLUS)
# Qt 4.8
add_definitions (-DQT_USE_QSTRINGBUILDER)

if (MINGW)
	# override cmake bug/feature?
	set (CMAKE_SHARED_LIBRARY_PREFIX "")
endif (MINGW)

# warnings and other flags
if (NOT MSVC)
	if (ENABLE_DEVELOPER_BUILD)
		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			set (CMAKE_C_FLAGS "-fcatch-undefined-behavior ${CMAKE_C_FLAGS}")
			set (CMAKE_CXX_FLAGS "-fcatch-undefined-behavior ${CMAKE_CXX_FLAGS}")
		endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

		set (CMAKE_C_FLAGS "-Werror ${CMAKE_C_FLAGS}")
		set (CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
	endif (ENABLE_DEVELOPER_BUILD)

	set (CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra -Woverloaded-virtual ${CMAKE_CXX_FLAGS}")
	if (MINGW)
		set (CMAKE_C_FLAGS "-mtune=i686 -pipe ${CMAKE_C_FLAGS}")
		set (CMAKE_CXX_FLAGS "-mtune=i686 -pipe ${CMAKE_CXX_FLAGS}")
	endif (MINGW)
else (NOT MSVC)
	set (CMAKE_C_FLAGS "/MP /Zc:wchar_t- ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "/MP /Zc:wchar_t- ${CMAKE_CXX_FLAGS}")
	add_definitions (/D_CRT_SECURE_NO_WARNINGS=1)
endif (NOT MSVC)

# installation paths

if (NOT KADU_BINDIR)
	if (WIN32)
		set (KADU_BINDIR ${CMAKE_INSTALL_PREFIX})
	else (WIN32)
		set (KADU_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
	endif (WIN32)
endif (NOT KADU_BINDIR)

if (NOT KADU_DATADIR)
	if (WIN32)
		set (KADU_DATADIR ${CMAKE_INSTALL_PREFIX})
	else (WIN32)
		set (KADU_DATADIR ${CMAKE_INSTALL_PREFIX}/share/kadu)
	endif (WIN32)
endif (NOT KADU_DATADIR)

if (NOT KADU_PLUGINS_LIBDIR)
	if (WIN32)
		set (KADU_PLUGINS_LIBDIR ${CMAKE_INSTALL_PREFIX}/plugins)
	else (WIN32)
		set (KADU_PLUGINS_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/kadu/plugins)
	endif (WIN32)
endif (NOT KADU_PLUGINS_LIBDIR)

if (NOT KADU_PLUGINS_DIR)
	set (KADU_PLUGINS_DIR "${KADU_DATADIR}/plugins")
endif (NOT KADU_PLUGINS_DIR)
if (NOT KADU_PLUGINS_DIR STREQUAL "${KADU_DATADIR}/plugins")
	message (WARNING "Custom KADU_PLUGINS_DIR directories are not supported by Kadu source code. If you really need it, please report a feature request to the Kadu project.")
endif (NOT KADU_PLUGINS_DIR STREQUAL "${KADU_DATADIR}/plugins")

if (NOT KADU_SDK_DIR)
	set (KADU_SDK_DIR ${CMAKE_INSTALL_PREFIX}/sdk)
endif (NOT KADU_SDK_DIR)

if (NOT KADU_INSTALL_INCLUDE_DIR)
	if (WIN32)
		set (KADU_INSTALL_INCLUDE_DIR ${KADU_SDK_DIR}/include)
	else (WIN32)
		set (KADU_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/kadu)
	endif (WIN32)
endif (NOT KADU_INSTALL_INCLUDE_DIR)

if (NOT KADU_CMAKE_MODULES_DIR)
	if (WIN32)
		set (KADU_CMAKE_MODULES_DIR ${KADU_SDK_DIR}/cmake/Modules)
	else (WIN32)
		set (KADU_CMAKE_MODULES_DIR ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules)
	endif (WIN32)
endif (NOT KADU_CMAKE_MODULES_DIR)

if (UNIX AND NOT APPLE)
	if (NOT KADU_DESKTOP_FILE_DIR)
		set (KADU_DESKTOP_FILE_DIR ${CMAKE_INSTALL_PREFIX}/share/applications)
	endif (NOT KADU_DESKTOP_FILE_DIR)
	if (NOT KADU_DESKTOP_FILE_NAME)
		set (KADU_DESKTOP_FILE_NAME kadu.desktop)
	endif (NOT KADU_DESKTOP_FILE_NAME)
endif (UNIX AND NOT APPLE)

file (RELATIVE_PATH KADU_DATADIR_RELATIVE_TO_BIN "${KADU_BINDIR}" "${KADU_DATADIR}")
file (RELATIVE_PATH KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "${KADU_BINDIR}" "${KADU_PLUGINS_LIBDIR}")
if (UNIX AND NOT APPLE)
	file (RELATIVE_PATH KADU_DESKTOP_FILE_DIR_RELATIVE_TO_BIN "${KADU_BINDIR}" "${KADU_DESKTOP_FILE_DIR}")
endif (UNIX AND NOT APPLE)

macro (kadu_api_directories INCLUDE_DIR)
	if (NOT KADU_INSTALLS_SDK)
		message (FATAL_ERROR "kadu_api_directories called but KADU_INSTALLS_SDK is not set")
	endif (NOT KADU_INSTALLS_SDK)

	if (INSTALL_SDK)
		foreach (ARG ${ARGN})
			file (GLOB API_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}/*.h)
			install (FILES ${API_HEADERS} DESTINATION ${KADU_INSTALL_INCLUDE_DIR}/${INCLUDE_DIR}/${ARG})
		endforeach (ARG)
	endif (INSTALL_SDK)
endmacro (kadu_api_directories)

macro (kadu_configuration)
	install (FILES ${ARGN}
		DESTINATION ${KADU_DATADIR}/configuration
	)
endmacro (kadu_configuration)

macro (kadu_plugin_configuration)
	install (FILES ${ARGN}
		DESTINATION ${KADU_PLUGINS_DIR}/configuration
	)
endmacro (kadu_plugin_configuration)

macro (kadu_plugin_desc PLUGIN_DESC)
	install (FILES ${ARGN}
		DESTINATION ${KADU_PLUGINS_DIR}
	)
endmacro (kadu_plugin_desc)

macro (kadu_plugin)
	set (VARIABLE_NAME "PLUGIN_NAME")

	set (PLUGIN_NAME "")
	set (PLUGIN_SOURCES "")
	set (PLUGIN_MOC_SOURCES "")
	set (PLUGIN_TRANSLATION_SOURCES "")
	set (PLUGIN_CONFIGURATION_FILES "")
	set (PLUGIN_DATA_FILES "")
	set (PLUGIN_DATA_DIRECTORY "")
	set (PLUGIN_BUILDDEF "")
	set (PLUGIN_DEPENDENCIES "")
	set (PLUGIN_LIBRARIES "")

	include_directories (".")
	include_directories ("${KADU_INCLUDE_DIR}")
	include_directories ("${KADU_INCLUDE_DIR}/kadu-core")

	foreach (ARG ${ARGV})
		if ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
			set (PLUGIN_NAME ${ARG})
			set (VARIABLE_NAME "")
		elseif (ARG MATCHES "(PLUGIN_SOURCES|PLUGIN_MOC_SOURCES|PLUGIN_TRANSLATION_SOURCES|PLUGIN_CONFIGURATION_FILES|PLUGIN_DATA_FILES|PLUGIN_DATA_DIRECTORY|PLUGIN_BUILDDEF|PLUGIN_DEPENDENCIES|PLUGIN_LIBRARIES)")
			set (VARIABLE_NAME ${ARG})
		elseif (VARIABLE_NAME STREQUAL "")
			message (FATAL_ERROR "Invalid invocation of kadu_plugin macro")
		else ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
			set (${VARIABLE_NAME} "${${VARIABLE_NAME}};${ARG}")
		endif ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
	endforeach (ARG)

	if (WIN32)
		include_directories ("${KADU_CURRENT_SDK_DIR}" "${KADU_CURRENT_SDK_DIR}/plugins")
		link_directories ("${KADU_CURRENT_SDK_DIR}/lib")

		list (APPEND PLUGIN_SOURCES ${PLUGIN_NAME}.rc)
		add_custom_command (OUTPUT ${PLUGIN_NAME}.rc
			COMMAND "${KADU_CURRENT_SDK_DIR}/plugins/pluginrcgen.bat"
			ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.desc ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.rc
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.desc
			COMMENT "Building RC source ${PLUGIN_NAME}.rc"
		)
	endif (WIN32)

	install (FILES ${PLUGIN_NAME}.desc
		DESTINATION ${KADU_PLUGINS_DIR}
	)

	if (PLUGIN_MOC_SOURCES)
		qt4_wrap_cpp (PLUGIN_MOC_FILES ${PLUGIN_MOC_SOURCES})
	endif (PLUGIN_MOC_SOURCES)

	if (PLUGIN_TRANSLATION_SOURCES)
		qt4_add_translation (PLUGIN_TRANSLATION_FILES ${PLUGIN_TRANSLATION_SOURCES})
	endif (PLUGIN_TRANSLATION_SOURCES)

	kadu_plugin_configuration (${PLUGIN_CONFIGURATION_FILES})

	install (FILES ${PLUGIN_TRANSLATION_FILES}
		DESTINATION ${KADU_PLUGINS_DIR}/translations
	)

	if (NOT "${PLUGIN_DATA_FILES}" STREQUAL "")
		install (FILES ${PLUGIN_DATA_FILES}
			DESTINATION ${KADU_PLUGINS_DIR}/data/${PLUGIN_NAME}
		)
	endif (NOT "${PLUGIN_DATA_FILES}" STREQUAL "")

	if (NOT "${PLUGIN_DATA_DIRECTORY}" STREQUAL "")
		install (DIRECTORY ${PLUGIN_DATA_DIRECTORY}
			DESTINATION ${KADU_PLUGINS_DIR}/data/${PLUGIN_NAME}
		)
	endif (NOT "${PLUGIN_DATA_DIRECTORY}" STREQUAL "")

	add_library (${PLUGIN_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_MOC_FILES})
	add_custom_target (${PLUGIN_NAME}-translations DEPENDS ${PLUGIN_TRANSLATION_FILES})

	add_dependencies (${PLUGIN_NAME} ${PLUGIN_NAME}-translations)

	set_target_properties (${PLUGIN_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")

	if (NOT ${PLUGIN_BUILDDEF} STREQUAL "")
		set_target_properties (${PLUGIN_NAME} PROPERTIES COMPILE_DEFINITIONS ${PLUGIN_BUILDDEF})
	endif (NOT ${PLUGIN_BUILDDEF} STREQUAL "")

	if (NOT "${PLUGIN_LIBRARIES}" STREQUAL "")
		target_link_libraries (${PLUGIN_NAME} ${PLUGIN_LIBRARIES})
	endif (NOT "${PLUGIN_LIBRARIES}" STREQUAL "")

	if (WIN32)
		target_link_libraries (${PLUGIN_NAME} kadu_core ${PLUGIN_DEPENDENCIES} ${QT_LIBRARIES})

		if (INSTALL_SDK)
			install (TARGETS ${PLUGIN_NAME} ARCHIVE DESTINATION ${KADU_SDK_DIR}/lib)
		endif (INSTALL_SDK)
	endif (WIN32)

	if (APPLE)
		set_target_properties (${PLUGIN_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)

	install (TARGETS ${PLUGIN_NAME} RUNTIME DESTINATION ${KADU_PLUGINS_LIBDIR} LIBRARY DESTINATION ${KADU_PLUGINS_LIBDIR})

	if (NOT MSVC)
		cmake_policy(SET CMP0002 OLD)
		if (NOT TARGET tsupdate)
			add_custom_target (tsupdate)
		endif (NOT TARGET tsupdate)
		add_custom_target (${PLUGIN_NAME}-tsupdate
			"${KADU_CURRENT_SDK_DIR}/translations/plugintsupdate.sh" "${CMAKE_CURRENT_SOURCE_DIR}"
		)
		add_dependencies (tsupdate ${PLUGIN_NAME}-tsupdate)
		cmake_policy(SET CMP0002 NEW)
	endif (NOT MSVC)
endmacro (kadu_plugin)
