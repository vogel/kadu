option (BUILD_PLUGINS "Switch to OFF if you do not want to build plugins" ON)

if (BUILD_PLUGINS)
	if (NOT WIN32)
		find_program (AWK gawk awk)
		if (AWK)
			set (_COMPILE_PLUGINS "")
			foreach (ARG ${COMPILE_PLUGINS})
				set (_COMPILE_PLUGINS "${_COMPILE_PLUGINS} ${ARG}")
			endforeach ()

			exec_program (${CMAKE_CURRENT_SOURCE_DIR}/check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
				ARGS ${AWK} ${_COMPILE_PLUGINS}
				OUTPUT_VARIABLE DEPS_MESSAGE
				RETURN_VALUE DEPS_STATUS
			)

			if (DEPS_STATUS EQUAL 1)
				message (FATAL_ERROR ${DEPS_MESSAGE})
			endif ()
		endif ()
	endif ()

	foreach (PLUGIN ${COMPILE_PLUGINS})
		message (STATUS "Plugin: " ${PLUGIN})
		add_subdirectory (${PLUGIN})
	endforeach ()
endif ()

if (KADU_INSTALL_SDK)
	if (WIN32)
		install (PROGRAMS pluginrcgen.bat DESTINATION "${KADU_INSTALL_SDK_DIR}/plugins")
		install (FILES pluginbase.rc DESTINATION "${KADU_INSTALL_SDK_DIR}/plugins")
	endif ()

	install (FILES PluginConfig.cmake.in DESTINATION "${KADU_INSTALL_SDK_DIR}/plugins")
endif ()
