option (BUILD_PLUGINS "Switch to OFF if you do not want to build plugins" ON)

if (BUILD_PLUGINS)
	if (NOT WIN32)
		find_program (AWK gawk awk)
		if (AWK)
			set (_COMPILE_PLUGINS "")
			foreach (ARG ${COMPILE_PLUGINS})
				set (_COMPILE_PLUGINS "${_COMPILE_PLUGINS} ${ARG}")
			endforeach (ARG)

			exec_program (${CMAKE_CURRENT_SOURCE_DIR}/check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS ${AWK} ${_COMPILE_PLUGINS}
				OUTPUT_VARIABLE DEPS_MESSAGE
				RETURN_VALUE DEPS_STATUS
			)

			if (DEPS_STATUS EQUAL 1)
				message (FATAL_ERROR ${DEPS_MESSAGE})
			endif (DEPS_STATUS EQUAL 1)
		endif (AWK)
	endif (NOT WIN32)

	foreach (PLUGIN ${COMPILE_PLUGINS})
		message (STATUS "Plugin: " ${PLUGIN})
		add_subdirectory (${PLUGIN})
	endforeach (PLUGIN)
endif (BUILD_PLUGINS)

if (WIN32 AND INSTALL_SDK)
	install (PROGRAMS pluginrcgen.bat DESTINATION "${KADU_SDK_DIR}/plugins")
	install (FILES pluginbase.rc DESTINATION "${KADU_SDK_DIR}/plugins")
endif (WIN32 AND INSTALL_SDK)
