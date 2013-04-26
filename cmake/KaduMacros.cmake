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
cmake_minimum_required (VERSION 2.8.9)
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

set (CMAKE_INSTALL_PREFIX "${KADU_INSTALL_PREFIX}" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)

# Set default build type
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "${KADU_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# To be used on each target
macro (kadu_set_flags _target)
	if (NOT TARGET ${_target})
		message (FATAL_ERROR "kadu_use called with non-existning target as parameter")
	endif ()

	set_property (TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS ${KADU_DEFINITIONS})
	set_property (TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG ${KADU_DEFINITIONS_DEBUG})

	if (KADU_COMPILE_FLAGS)
		set_property (TARGET ${_target} APPEND_STRING PROPERTY COMPILE_FLAGS " ${KADU_COMPILE_FLAGS}")
	endif ()

	if (KADU_LINK_FLAGS)
		set_property (TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS " ${KADU_LINK_FLAGS}")
	endif ()

	get_target_property (_sources ${_target} SOURCES)
	if (KADU_C_FLAGS OR KADU_CXX_FLAGS)
		# If all sources are CXX, we could simply set flags on whole target,
		# not on individual source files. This would greatly simplify the underlying
		# build system rules, but is probably not necessary.
		foreach (_source ${_sources})
			get_source_file_property (_lang ${_source} LANGUAGE)
			if (KADU_C_FLAGS AND _lang STREQUAL "C")
				set_property (SOURCE ${_source} APPEND_STRING PROPERTY COMPILE_FLAGS " ${KADU_C_FLAGS}")
			elseif (KADU_CXX_FLAGS AND _lang STREQUAL "CXX")
				set_property (SOURCE ${_source} APPEND_STRING PROPERTY COMPILE_FLAGS " ${KADU_CXX_FLAGS}")
			endif ()
		endforeach ()
	endif ()
endmacro ()

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
	set (PLUGIN_CONFIGURATION_FILES "")
	set (PLUGIN_DATA_FILES "")
	set (PLUGIN_DATA_DIRECTORIES "")
	set (PLUGIN_DEPENDENCIES "")
	set (PLUGIN_LIBRARIES "")

	include_directories (".")
	include_directories (${KADU_INCLUDE_DIRS})

	foreach (ARG ${ARGV})
		if ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
			set (PLUGIN_NAME ${ARG})
			set (VARIABLE_NAME "")
		elseif (ARG MATCHES "(PLUGIN_SOURCES|PLUGIN_MOC_SOURCES|PLUGIN_CONFIGURATION_FILES|PLUGIN_DATA_FILES|PLUGIN_DATA_DIRECTORIES|PLUGIN_DEPENDENCIES|PLUGIN_LIBRARIES)")
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

	add_library (${PLUGIN_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_MOC_FILES})
	kadu_set_flags (${PLUGIN_NAME})

	if (KADU_INSTALL_UNOFFICIAL_TRANSLATIONS)
		file (GLOB PLUGIN_TRANSLATION_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "translations/${PLUGIN_NAME}_*.ts")
	else ()
		set (PLUGIN_TRANSLATION_SOURCES)
		foreach (LANGUAGE ${KADU_ENABLED_LANGUAGES})
			set (file_ "${CMAKE_CURRENT_SOURCE_DIR}/translations/${PLUGIN_NAME}_${LANGUAGE}.ts")
			if (EXISTS "${file_}")
				list (APPEND PLUGIN_TRANSLATION_SOURCES "${file_}")
			endif ()
		endforeach ()
	endif ()

	if (PLUGIN_TRANSLATION_SOURCES)
		qt4_add_translation (PLUGIN_TRANSLATION_FILES ${PLUGIN_TRANSLATION_SOURCES})

		install (FILES ${PLUGIN_TRANSLATION_FILES}
			DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/translations
		)

		add_custom_target (${PLUGIN_NAME}-translations DEPENDS ${PLUGIN_TRANSLATION_FILES})
		add_dependencies (${PLUGIN_NAME} ${PLUGIN_NAME}-translations)
	endif ()

	install (FILES ${PLUGIN_CONFIGURATION_FILES}
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/configuration
	)

	if (NOT "${PLUGIN_DATA_FILES}" STREQUAL "")
		install (FILES ${PLUGIN_DATA_FILES}
			DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/${PLUGIN_NAME}
		)
	endif ()

	if (NOT "${PLUGIN_DATA_DIRECTORIES}" STREQUAL "")
		install (DIRECTORY ${PLUGIN_DATA_DIRECTORIES}
			DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/${PLUGIN_NAME}
		)
	endif ()

	if (NOT "${PLUGIN_LIBRARIES}" STREQUAL "")
		target_link_libraries (${PLUGIN_NAME} LINK_PRIVATE ${PLUGIN_LIBRARIES})
	endif ()

	if (WIN32)
		target_link_libraries (${PLUGIN_NAME} LINK_PRIVATE libkadu ${PLUGIN_DEPENDENCIES} ${QT_LIBRARIES})

		if (KADU_INSTALL_SDK)
			install (TARGETS ${PLUGIN_NAME} ARCHIVE DESTINATION ${KADU_INSTALL_SDK_DIR}/lib)
		endif ()
	endif ()

	if (APPLE)
		set_property (TARGET ${PLUGIN_NAME} APPEND_STRING PROPERTY LINK_FLAGS " -undefined dynamic_lookup")
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
