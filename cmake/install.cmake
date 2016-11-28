# Installation paths.

set (INSTALL_SDK_DIR "" CACHE PATH "SDK installation root")
mark_as_advanced (INSTALL_SDK_DIR)
if (NOT INSTALL_SDK_DIR)
	set (INSTALL_SDK_DIR "${CMAKE_INSTALL_PREFIX}/sdk")
	set (install_sdk_dir_set_to_default TRUE)
endif ()

include (GNUInstallDirs)

set (INSTALL_BIN_DIR "${CMAKE_INSTALL_FULL_BINDIR}")
set (INSTALL_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
set (INSTALL_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/kadu")
set (INSTALL_PLUGINS_DATA_DIR "${INSTALL_DATA_DIR}/plugins")
set (INSTALL_PLUGINS_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/kadu/plugins")
set (INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}/kadu")
set (INSTALL_CMAKE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/cmake/Kadu")

if (install_sdk_dir_set_to_default)
	set (INSTALL_SDK_DIR "${INSTALL_DATA_DIR}/sdk")
endif ()

if (WIN32)
	set (KADU_DATADIR_RELATIVE_TO_BIN "./")
	set (KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "./plugins/")
else ()
	file (RELATIVE_PATH KADU_DATADIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${INSTALL_DATA_DIR}")
	file (RELATIVE_PATH KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${INSTALL_PLUGINS_LIB_DIR}")
endif ()

if (UNIX AND NOT APPLE)
	set (DESKTOP_FILE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/applications")
	set (DESKTOP_FILE_NAME kadu.desktop)

	file (RELATIVE_PATH KADU_DESKTOP_FILE_DIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${DESKTOP_FILE_DIR}")
endif ()

set (install_vars
	INSTALL_SDK_DIR INSTALL_BIN_DIR INSTALL_LIB_DIR INSTALL_DATA_DIR INSTALL_PLUGINS_DATA_DIR
	INSTALL_PLUGINS_LIB_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR
)
foreach (install_var ${install_vars})
	if (NOT DEFINED ${install_var})
		message (FATAL_ERROR "Variable ${install_var} does not exist")
	else ()
		if (IS_ABSOLUTE "${${install_var}}")
			file (TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" sane_install_prefix)
			file (TO_CMAKE_PATH "${${install_var}}" sane_install_path)
			if (sane_install_path STREQUAL sane_install_prefix)
				set (RELATIVE_${install_var} "")
			else ()
				string (REPLACE "${sane_install_prefix}/" "" RELATIVE_${install_var} "${sane_install_path}")
			endif ()
			if (RELATIVE_${install_var} STREQUAL "")
				set (RELATIVE_${install_var} ".")
			endif ()

			if (IS_ABSOLUTE "${RELATIVE_${install_var}}")
				message (FATAL_ERROR "${install_var} (${${install_var}}) is not under install prefix.")
			endif ()
		else ()
			set (RELATIVE_${install_var} "${${install_var}}")
		endif()
	endif ()
endforeach ()

set (KADU_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set (path_vars KADU_INSTALL_PREFIX)
if (UNIX AND NOT APPLE)
	list (APPEND path_vars DESKTOP_FILE_DIR)
endif ()
