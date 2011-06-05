if (BUILD_PLUGINS)

	if (DEPS_STATUS EQUAL 1)
		message (FATAL_ERROR ${DEPS_MESSAGE})
	endif (DEPS_STATUS EQUAL 1)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${AWK} ${COMPILE_PLUGINS}
		OUTPUT_VARIABLE DEPS_MESSAGE
		RETURN_VALUE DEPS_STATUS
	)

	foreach (PLUGIN ${COMPILE_PLUGINS})
		message (STATUS "Plugin: " ${PLUGIN})
		if (WIN32)
			set (${PLUGIN} SHARED)
		else (WIN32)
			set (${PLUGIN} PLUGIN)
		endif (WIN32)
		add_subdirectory (${PLUGIN})
	endforeach (PLUGIN)

endif (BUILD_PLUGINS)
