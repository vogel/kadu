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

# Set default install prefix
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${KADU_INSTALL_PREFIX}" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif ()

# Set default build type
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "${KADU_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

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

include (CMakeParseArguments)

macro (kadu_plugin KADU_PLUGIN_NAME)
	set (_multi_value_keywords
		PLUGIN_SOURCES
		PLUGIN_MOC_SOURCES
		PLUGIN_CONFIGURATION_FILES
		PLUGIN_DATA_FILES
		PLUGIN_DATA_DIRECTORIES
		PLUGIN_DEPENDENCIES
		PLUGIN_LIBRARIES
	)

	cmake_parse_arguments (KADU "" "" "${_multi_value_keywords}" ${ARGN})

	if (KADU_UNPARSED_ARGUMENTS)
		message (FATAL_ERROR "Unknown keywords given to kadu_plugin(): \"${KADU_UNPARSED_ARGUMENTS}\"")
	endif()

	include_directories (".")
	include_directories (${KADU_INCLUDE_DIRS})

	if (WIN32)
		include_directories ("${KADU_SDK_DIR}" "${KADU_SDK_DIR}/plugins")

		list (APPEND KADU_PLUGIN_SOURCES ${KADU_PLUGIN_NAME}.rc)
		add_custom_command (OUTPUT ${KADU_PLUGIN_NAME}.rc
			COMMAND "${KADU_SDK_DIR}/plugins/pluginrcgen.bat"
			ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${KADU_PLUGIN_NAME}.desc ${CMAKE_CURRENT_BINARY_DIR}/${KADU_PLUGIN_NAME}.rc
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${KADU_PLUGIN_NAME}.desc
			COMMENT "Building RC source ${KADU_PLUGIN_NAME}.rc"
		)
	endif ()

	install (FILES ${KADU_PLUGIN_NAME}.desc
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}
	)

	qt4_wrap_cpp (_moc_files ${KADU_PLUGIN_MOC_SOURCES})
	add_library (${KADU_PLUGIN_NAME} SHARED ${KADU_PLUGIN_SOURCES} ${_moc_files})
	kadu_set_flags (${KADU_PLUGIN_NAME})

	if (KADU_INSTALL_UNOFFICIAL_TRANSLATIONS)
		file (GLOB _translation_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "translations/${KADU_PLUGIN_NAME}_*.ts")
	else ()
		set (_translation_sources)
		foreach (LANGUAGE ${KADU_ENABLED_LANGUAGES})
			set (file_ "${CMAKE_CURRENT_SOURCE_DIR}/translations/${KADU_PLUGIN_NAME}_${LANGUAGE}.ts")
			if (EXISTS "${file_}")
				list (APPEND _translation_sources "${file_}")
			endif ()
		endforeach ()
	endif ()

	if (_translation_sources)
		qt4_add_translation (_translation_files ${_translation_sources})

		install (FILES ${_translation_files}
			DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/translations
		)

		add_custom_target (${KADU_PLUGIN_NAME}-translations DEPENDS ${_translation_files})
		add_dependencies (${KADU_PLUGIN_NAME} ${KADU_PLUGIN_NAME}-translations)
	endif ()

	install (FILES ${KADU_PLUGIN_CONFIGURATION_FILES}
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/configuration
	)

	install (FILES ${KADU_PLUGIN_DATA_FILES}
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/${KADU_PLUGIN_NAME}
	)

	install (DIRECTORY ${KADU_PLUGIN_DATA_DIRECTORIES}
		DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/${KADU_PLUGIN_NAME}
	)

	target_link_libraries (${KADU_PLUGIN_NAME} LINK_PRIVATE
		${KADU_LIBRARIES} ${KADU_PLUGIN_DEPENDENCIES} ${KADU_PLUGIN_LIBRARIES} ${QT_LIBRARIES}
	)

	install (TARGETS ${KADU_PLUGIN_NAME}
		RUNTIME DESTINATION ${KADU_INSTALL_PLUGINS_LIB_DIR}
		LIBRARY DESTINATION ${KADU_INSTALL_PLUGINS_LIB_DIR}
	)
	if (WIN32)
		if (KADU_INSTALL_SDK)
			install (TARGETS ${KADU_PLUGIN_NAME} ARCHIVE DESTINATION ${KADU_INSTALL_SDK_DIR}/lib)
		endif ()
	endif ()

	if (NOT MSVC)
		cmake_policy (SET CMP0002 OLD)
		if (NOT TARGET tsupdate)
			add_custom_target (tsupdate)
		endif ()
		add_custom_target (${KADU_PLUGIN_NAME}-tsupdate
			"${KADU_SDK_DIR}/translations/plugintsupdate.sh" "${CMAKE_CURRENT_SOURCE_DIR}"
		)
		add_dependencies (tsupdate ${KADU_PLUGIN_NAME}-tsupdate)
		cmake_policy (SET CMP0002 NEW)
	endif ()
endmacro ()
