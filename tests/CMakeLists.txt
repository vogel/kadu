find_package (Qt REQUIRED COMPONENTS QtTest)

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

	add_executable (${name_} ${ARGN})
	target_link_libraries (${name_} libkadu ${QT_QTTEST_LIBRARY})
	add_test ("${sourcePath}/${name_}" ${name_})

	if ("${sourcePath}" MATCHES "^[^/]+/plugins/.*")
		string (REGEX REPLACE "^[^/]+/plugins/" "" pluginName "${sourcePath}")
		string (REGEX REPLACE "^[^/]*/.*" "" pluginName "${pluginName}")
		if (NOT "${pluginName}" STREQUAL "" AND TARGET ${pluginName})
			target_link_libraries (${name_} ${pluginName})
		endif ()
	endif ()
endmacro ()

add_subdirectory (auto)
