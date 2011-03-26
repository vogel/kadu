set (BLACKLIST_PLUGINS
	advanced_userlist
	dbus
	echo
	profiles
	split_messages
	tlen_protocol
	weather
)

if (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS OFF
		OUTPUT_VARIABLE PLUGINS
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.bat ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${PLUGINS}
		OUTPUT_VARIABLE DYNAMIC_PLUGINS
	)

	if (ENABLE_AUTODOWNLOAD)
		foreach (PLUGIN ${DYNAMIC_PLUGINS})
			if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}.web)
				if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/.autodownloaded OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt)
					exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload.bat ${CMAKE_CURRENT_SOURCE_DIR}
						ARGS plugin ${PLUGIN}
						RETURN_VALUE DOWNLOAD_STATUS
					)
					if (DOWNLOAD_STATUS EQUAL 1)
						message (FATAL_ERROR ${PLUGIN} " download failed!")
					endif (DOWNLOAD_STATUS EQUAL 1)
				endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/.autodownloaded OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt)
			endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}.web)
		endforeach (PLUGIN)
	endif (ENABLE_AUTODOWNLOAD)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${AWK} ${DYNAMIC_PLUGINS}
		OUTPUT_VARIABLE DEPS_MESSAGE
		RETURN_VALUE DEPS_STATUS
	)

	if (DEPS_STATUS EQUAL 1)
		message (FATAL_ERROR ${DEPS_MESSAGE})
	endif (DEPS_STATUS EQUAL 1)
else (WIN32)
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTODOWNLOAD}
		OUTPUT_VARIABLE PLUGINS
	)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${PLUGINS}
		OUTPUT_VARIABLE DYNAMIC_PLUGINS
	)

	# autodownload
	if (ENABLE_AUTODOWNLOAD)
		foreach (PLUGIN ${DYNAMIC_PLUGINS})
			if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}.web)
				if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/.autodownloaded OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt)
					exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload ${CMAKE_CURRENT_SOURCE_DIR}
						ARGS plugin ${PLUGIN}
						RETURN_VALUE DOWNLOAD_STATUS
					)
					if (DOWNLOAD_STATUS EQUAL 1)
						message (FATAL_ERROR ${PLUGIN} " download failed!")
					endif (DOWNLOAD_STATUS EQUAL 1)
				endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/.autodownloaded OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}/CMakeLists.txt)
			endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}.web)
		endforeach (PLUGIN)
	endif (ENABLE_AUTODOWNLOAD)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${AWK} ${DYNAMIC_PLUGINS}
		OUTPUT_VARIABLE DEPS_MESSAGE
		RETURN_VALUE DEPS_STATUS
	)

	if (DEPS_STATUS EQUAL 1)
		message (FATAL_ERROR ${DEPS_MESSAGE})
	endif (DEPS_STATUS EQUAL 1)

	find_program (AWK NAMES awk gawk)
endif (WIN32)

MACRO(LIST_CONTAINS var value)
	SET(${var})
	FOREACH (value2 ${ARGN})
		IF (${value} STREQUAL ${value2})
			SET(${var} TRUE)
		ENDIF (${value} STREQUAL ${value2})
	ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

if (NOT DYNAMIC_PLUGINS STREQUAL "\n")
	foreach (PLUGIN ${DYNAMIC_PLUGINS})
		LIST_CONTAINS(BLACKLISTED ${PLUGIN} ${BLACKLIST_PLUGINS})
		if (BLACKLISTED)
			message (STATUS "Plugin ${PLUGIN} is blacklisted")
		else (BLACKLISTED)
			message (STATUS "Plugin: " ${PLUGIN})
			if (WIN32)
				set (${PLUGIN} SHARED)
			else (WIN32)
				set (${PLUGIN} PLUGIN)
			endif (WIN32)
		# 	link_directories (${PLUGIN})
			add_subdirectory (${PLUGIN})
		endif (BLACKLISTED)
	endforeach (PLUGIN)
endif (NOT DYNAMIC_PLUGINS STREQUAL "\n")
