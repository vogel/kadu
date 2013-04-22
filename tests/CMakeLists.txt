find_package (Qt QUIET REQUIRED COMPONENTS QtTest)

set (CMAKE_INCLUDE_CURRENT_DIR TRUE)

# kadu_add_test (<name> source1 source2 ... sourceN)
macro (kadu_add_test name_)
	foreach (file ${ARGN})
		get_filename_component (mocFile "${file}" NAME_WE)
		set (mocFile "${CMAKE_CURRENT_BINARY_DIR}/${mocFile}.moc")
		qt4_generate_moc (${file} "${mocFile}")
		set_property (SOURCE ${file} APPEND PROPERTY OBJECT_DEPENDS "${mocFile}")
	endforeach ()

	file (RELATIVE_PATH sourcePath "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_CURRENT_SOURCE_DIR}")

	get_filename_component (exeDir "${CMAKE_CURRENT_BINARY_DIR}/${name_}" PATH)
	file (MAKE_DIRECTORY "${exeDir}")

	add_executable (${name_} ${ARGN})
	kadu_set_flags (${name_})
	add_test ("${sourcePath}/${name_}" ${name_})

	if ("${sourcePath}" MATCHES "^[^/]+/plugins/.*")
		string (REGEX REPLACE "^[^/]+/plugins/" "" pluginName "${sourcePath}")
		string (REGEX REPLACE "^[^/]*/.*" "" pluginName "${pluginName}")
		if (NOT "${pluginName}" STREQUAL "" AND TARGET ${pluginName})
			target_link_libraries (${name_} ${pluginName})
		endif ()
	endif ()

	# Add it after the plugin so that --as-needed won't drop anything
	# needed by the plugin.
	target_link_libraries (${name_} libkadu ${QT_QTTEST_LIBRARY})
endmacro ()

add_subdirectory (auto)
