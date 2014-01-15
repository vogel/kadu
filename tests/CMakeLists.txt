find_package (Qt4 QUIET REQUIRED COMPONENTS QtTest)

include_directories(".")

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

	get_filename_component (exeDir "${CMAKE_CURRENT_BINARY_DIR}/${name_}" PATH)
	file (MAKE_DIRECTORY "${exeDir}")

	add_executable (${name_} ${ARGN})
	kadu_set_flags (${name_})
	if (NOT MSVC)
		set_property (TARGET ${name_} APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-error")
	endif ()
	add_test ("${sourcePath}/${name_}" ${name_})

	# Add libkadu after the plugin so that --as-needed won't drop anything
	# needed by the plugin.
	target_link_libraries (${name_} LINK_PRIVATE ${pluginName} libkadu ${QT_LIBRARIES} ${QT_QTTEST_LIBRARY})
endfunction ()

add_subdirectory (unit)
