# Try to find Kadu development headers
# Once done this will define
#
#  KADU_FOUND - system has Kadu headers
#  KADU_INCLUDE_DIR - the Kadu include directory
#
# Based on FindQutIM by Ruslan Nigmatullin, <euroelessar@gmail.com>
# Copyright (c) 2009, Ruslan Nigmatullin, <euroelessar@gmail.com>
# Copyrignt (c) 2011, Rafał 'Vogel' Malinowski <vogel@kadu.im>

# Do not require plugins to specify minimum version.
cmake_minimum_required (VERSION 2.8.5)
cmake_policy (SET CMP0000 OLD)

# libraries
set (QT_USE_QTXML 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
set (QT_USE_QTDECLARATIVE 1)
if (UNIX AND NOT APPLE)
	set (QT_USE_QTDBUS 1)
endif ()
find_package (Qt4 4.8.0 REQUIRED)
include (${QT_USE_FILE})

option (ENABLE_DEVELOPER_BUILD "Turn on some features helpful during development process (has nothing to do with debugging symbols)" OFF)

if (NOT MSVC AND NOT XCODE_VERSION AND NOT CMAKE_BUILD_TYPE)
	if (ENABLE_DEVELOPER_BUILD)
		set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
	else ()
		set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
	endif ()
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_DEVELOPER_BUILD OR WIN32)
	add_definitions (-DDEBUG_ENABLED)
endif ()

if (MINGW AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (-D_DEBUG)
endif ()

add_definitions (-DQT_USE_QSTRINGBUILDER)

# warnings and other flags
if (NOT MSVC)
	if (ENABLE_DEVELOPER_BUILD)
		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			# -fcatch-undefined-behavior generates trap on every undefined behavior by C/C++
			set (CMAKE_C_FLAGS "-fcatch-undefined-behavior ${CMAKE_C_FLAGS}")
			set (CMAKE_CXX_FLAGS "-fcatch-undefined-behavior ${CMAKE_CXX_FLAGS}")
		endif ()

		# -pipe can speed up the build
		# -ftrapv generates trap on signed integer overflow, which is undefined by C/C++
		# -fno-omit-frame-pointer gives potentially better stack traces at the cost of negligible performance drop
		set (CMAKE_C_FLAGS "-Werror -pipe -ftrapv -fno-omit-frame-pointer ${CMAKE_C_FLAGS}")
		set (CMAKE_CXX_FLAGS "-Werror -pipe -ftrapv -fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")

		if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "GNU")
			# -z now check there are no unresolved symbols at executalbe/library load time, instead of that specific symbol load time
			set (CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,now -Wl,--fatal-warnings ${CMAKE_SHARED_LINKER_FLAGS}")
			set (CMAKE_MODULE_LINKER_FLAGS "-Wl,-z,now -Wl,--fatal-warnings ${CMAKE_MODULE_LINKER_FLAGS}")
			set (CMAKE_EXE_LINKER_FLAGS "-Wl,-z,now -Wl,--fatal-warnings ${CMAKE_EXE_LINKER_FLAGS}")
		endif ()
	endif ()

	set (CMAKE_C_FLAGS "-Wall -Wextra -Wundef -Wcast-align -Wpointer-arith -Wwrite-strings -fno-common ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wundef -Wcast-align -Wpointer-arith -Woverloaded-virtual -Wnon-virtual-dtor -fno-common -fno-exceptions -DQT_NO_EXCEPTIONS ${CMAKE_CXX_FLAGS}")
else ()
	set (CMAKE_C_FLAGS "/MP /Zc:wchar_t- ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "/MP /Zc:wchar_t- ${CMAKE_CXX_FLAGS}")
	add_definitions (/D_CRT_SECURE_NO_WARNINGS=1)
endif ()

macro (kadu_api_directories INCLUDE_DIR)
	if (KADU_INSTALL_SDK)
		foreach (ARG ${ARGN})
			file (GLOB API_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}/*.h)
			install (FILES ${API_HEADERS} DESTINATION ${KADU_INSTALL_INCLUDE_DIR}/${INCLUDE_DIR}/${ARG})
		endforeach ()
	endif ()
endmacro ()

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
	include_directories (${KADU_INCLUDE_DIRS})

	foreach (ARG ${ARGV})
		if ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
			set (PLUGIN_NAME ${ARG})
			set (VARIABLE_NAME "")
		elseif (ARG MATCHES "(PLUGIN_SOURCES|PLUGIN_MOC_SOURCES|PLUGIN_TRANSLATION_SOURCES|PLUGIN_CONFIGURATION_FILES|PLUGIN_DATA_FILES|PLUGIN_DATA_DIRECTORY|PLUGIN_BUILDDEF|PLUGIN_DEPENDENCIES|PLUGIN_LIBRARIES)")
			set (VARIABLE_NAME ${ARG})
		elseif (VARIABLE_NAME STREQUAL "")
			message (FATAL_ERROR "Invalid invocation of kadu_plugin macro")
		else ()
			set (${VARIABLE_NAME} "${${VARIABLE_NAME}};${ARG}")
		endif ()
	endforeach ()

	if (WIN32)
		include_directories ("${KADU_SDK_DIR}" "${KADU_SDK_DIR}/plugins")
		link_directories ("${KADU_SDK_DIR}/lib")

		list (APPEND PLUGIN_SOURCES ${PLUGIN_NAME}.rc)
		add_custom_command (OUTPUT ${PLUGIN_NAME}.rc
			COMMAND "${KADU_SDK_DIR}/plugins/pluginrcgen.bat"
			ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.desc ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.rc
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.desc
			COMMENT "Building RC source ${PLUGIN_NAME}.rc"
		)
	endif ()

	install (FILES ${PLUGIN_NAME}.desc
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}
	)

	if (PLUGIN_MOC_SOURCES)
		qt4_wrap_cpp (PLUGIN_MOC_FILES ${PLUGIN_MOC_SOURCES})
	endif ()

	if (NOT PLUGIN_TRANSLATION_SOURCES)
		file (GLOB PLUGIN_TRANSLATION_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "translations/${PLUGIN_NAME}_*.ts")
	endif ()
	if (PLUGIN_TRANSLATION_SOURCES)
		qt4_add_translation (PLUGIN_TRANSLATION_FILES ${PLUGIN_TRANSLATION_SOURCES})
	endif ()

	install (FILES ${PLUGIN_CONFIGURATION_FILES}
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/configuration
	)

	install (FILES ${PLUGIN_TRANSLATION_FILES}
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/translations
	)

	if (NOT "${PLUGIN_DATA_FILES}" STREQUAL "")
		install (FILES ${PLUGIN_DATA_FILES}
			DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/${PLUGIN_NAME}
		)
	endif ()

	if (NOT "${PLUGIN_DATA_DIRECTORY}" STREQUAL "")
		install (DIRECTORY ${PLUGIN_DATA_DIRECTORY}
			DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/${PLUGIN_NAME}
		)
	endif ()

	add_library (${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES} ${PLUGIN_MOC_FILES})
	add_custom_target (${PLUGIN_NAME}-translations DEPENDS ${PLUGIN_TRANSLATION_FILES})

	add_dependencies (${PLUGIN_NAME} ${PLUGIN_NAME}-translations)

	set_target_properties (${PLUGIN_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")

	if (NOT ${PLUGIN_BUILDDEF} STREQUAL "")
		set_target_properties (${PLUGIN_NAME} PROPERTIES COMPILE_DEFINITIONS ${PLUGIN_BUILDDEF})
	endif ()

	if (NOT "${PLUGIN_LIBRARIES}" STREQUAL "")
		target_link_libraries (${PLUGIN_NAME} ${PLUGIN_LIBRARIES})
	endif ()

	if (WIN32)
		target_link_libraries (${PLUGIN_NAME} libkadu ${PLUGIN_DEPENDENCIES} ${QT_LIBRARIES})

		if (KADU_INSTALL_SDK)
			install (TARGETS ${PLUGIN_NAME} ARCHIVE DESTINATION ${KADU_INSTALL_SDK_DIR}/lib)
		endif ()
	endif ()

	if (APPLE)
		set_target_properties (${PLUGIN_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif ()

	install (TARGETS ${PLUGIN_NAME} RUNTIME DESTINATION ${KADU_INSTALL_PLUGINS_LIB_DIR} LIBRARY DESTINATION ${KADU_INSTALL_PLUGINS_LIB_DIR})

	if (NOT MSVC)
		cmake_policy(SET CMP0002 OLD)
		if (NOT TARGET tsupdate)
			add_custom_target (tsupdate)
		endif ()
		add_custom_target (${PLUGIN_NAME}-tsupdate
			"${KADU_SDK_DIR}/translations/plugintsupdate.sh" "${CMAKE_CURRENT_SOURCE_DIR}"
		)
		add_dependencies (tsupdate ${PLUGIN_NAME}-tsupdate)
		cmake_policy(SET CMP0002 NEW)
	endif ()
endmacro ()
