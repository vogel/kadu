# Generate CMake configuration files

include (CMakePackageConfigHelpers)

macro (kadu_configure_package_config_file _in _out _mode)
	if ("${_mode}" STREQUAL BUILD_TREE)
		foreach (path_var ${path_vars})
			set (PACKAGE_${path_var} "${${path_var}}")
		endforeach ()

		set (PACKAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
		set (PACKAGE_SDK_DIR "${CMAKE_SOURCE_DIR}")

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	elseif ("${_mode}" STREQUAL INSTALL_TREE)
		set (INCLUDE_DIR "${INSTALL_INCLUDE_DIR}")
		set (SDK_DIR "${INSTALL_SDK_DIR}")
		list (APPEND path_vars INCLUDE_DIR SDK_DIR)

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			PATH_VARS ${path_vars}
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	endif ()
endmacro ()

# This one is only for usable within the build tree. We want to actually
# set some variables in there from within the subdirectories, so we generated
# the intallation version later.
kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
	"${CMAKE_BINARY_DIR}/KaduConfig.cmake" BUILD_TREE)

write_basic_package_version_file ("${CMAKE_BINARY_DIR}/KaduConfigVersion.cmake"
	VERSION ${simple_version} COMPATIBILITY AnyNewerVersion
)

# Include the generated file

set (Kadu_DIR "${CMAKE_BINARY_DIR}")
find_package (Kadu REQUIRED CONFIG)
