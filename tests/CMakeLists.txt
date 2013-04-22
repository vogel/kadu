find_package (Qt QUIET REQUIRED COMPONENTS QtTest)

set (CMAKE_INCLUDE_CURRENT_DIR TRUE)

# kadu_add_test (<name> source1 source2 ... sourceN)
function (kadu_add_test name_)
	set (pluginName)
	file (RELATIVE_PATH sourcePath "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_CURRENT_SOURCE_DIR}")
	if ("${sourcePath}" MATCHES "^[^/]+/plugins/.*")
		string (REGEX REPLACE "^[^/]+/plugins/" "" pluginName "${sourcePath}")
		string (REGEX REPLACE "^[^/]*/.*" "" pluginName "${pluginName}")
		# Do not add the test if it tests a plugin we do not have.
		if (NOT "${pluginName}" STREQUAL "" AND NOT TARGET ${pluginName})
			return ()
		endif ()
	endif ()

	foreach (file ${ARGN})
		get_filename_component (mocFile "${file}" NAME_WE)
		set (mocFile "${CMAKE_CURRENT_BINARY_DIR}/${mocFile}.moc")
		qt4_generate_moc (${file} "${mocFile}")
		set_property (SOURCE ${file} APPEND PROPERTY OBJECT_DEPENDS "${mocFile}")
	endforeach ()

	get_filename_component (exeDir "${CMAKE_CURRENT_BINARY_DIR}/${name_}" PATH)
	file (MAKE_DIRECTORY "${exeDir}")

	add_executable (${name_} ${ARGN})
	kadu_set_flags (${name_})
	add_test ("${sourcePath}/${name_}" ${name_})

	# Add libkadu after the plugin so that --as-needed won't drop anything
	# needed by the plugin.
	target_link_libraries (${name_} ${pluginName} libkadu ${QT_QTTEST_LIBRARY})
endfunction ()

add_subdirectory (auto)
