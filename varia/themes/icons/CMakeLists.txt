set (ICON_THEMES
	default
	faenza
	glass
	oxygen
)

if (ICON_THEMES)
	add_custom_target (icon_themes
		DEPENDS ${ICON_THEMES}
	)
	add_dependencies (kadu icon_themes)

	#if (UNIX AND NOT APPLE)
	#	option (COMPRESS_SVG "Compress SVG files when installing (will break freedesktop_notify on notification-daemon, notify-osd and possibly other daemons, though will work on KDE)" OFF)
	#else ()
	#	option (COMPRESS_SVG "Compress SVG files when installing" ON)
	#endif ()

	#if (COMPRESS_SVG)
	#	message (STATUS "SVG icons compressing enabled")

	#	foreach (ICON_THEME ${ICON_THEMES})
	#		file (GLOB_RECURSE tmp RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	#			"${CMAKE_CURRENT_SOURCE_DIR}/${ICON_THEME}/*.svg"
	#		)
	#		list (APPEND inFiles ${tmp})
	#	endforeach ()

	#	if (inFiles)
	#		find_package (Gzip QUIET)
	#		if (NOT GZIP_FOUND)
	#			message (STATUS "Could not compress SVG files: gzip not found")
	#		else ()
	#			foreach (inFile ${inFiles})
	#				set (outFile "${CMAKE_CURRENT_BINARY_DIR}/svgzs/${inFile}z")
	#				list (APPEND outFiles "${outFile}")
	#				string (REGEX REPLACE "/[^/]*$" "" outFileDir "${outFile}")

	#				add_custom_command (OUTPUT "${outFile}"
	#					COMMAND "${CMAKE_COMMAND}" -E make_directory "${outFileDir}"
	#					COMMAND "${GZIP_EXECUTABLE}" --best -c "${CMAKE_CURRENT_SOURCE_DIR}/${inFile}" > "${outFile}"
	#					MAIN_DEPENDENCY "${inFile}"
	#					VERBATIM
	#				)
	#			endforeach ()

	#			add_custom_target (compress_svgs ALL DEPENDS ${outFiles})
	#			add_dependencies (icon_themes compress_svgs)
	#		endif ()
	#	endif ()
	#else ()
	#	message (STATUS "SVG icons compressing disabled")
	#endif ()

	foreach (ICON_THEME ${ICON_THEMES})
		message (STATUS "Icon theme: " ${ICON_THEME})
	endforeach ()

	install (DIRECTORY ${ICON_THEMES}
		DESTINATION "${KADU_INSTALL_DATA_DIR}/themes/icons"
		FILES_MATCHING
		PATTERN "*.gif"
		PATTERN "*.png"
		# PATTERN "*.svgz"
		# CMake has no option to exclude empty dirs (empty because of pattern matching above)
		PATTERN "src" EXCLUDE
	)

	#if (TARGET compress_svgs)
	#	install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/svgzs/"
	#		DESTINATION "${KADU_INSTALL_DATA_DIR}/themes/icons"
	#		FILES_MATCHING
	#		PATTERN "*.svgz"
	#	)
	#else ()
	#	install (DIRECTORY ${ICON_THEMES}
	#		DESTINATION "${KADU_INSTALL_DATA_DIR}/themes/icons"
	#		FILES_MATCHING
	#		PATTERN "*.svg"
	#		PATTERN "src" EXCLUDE
	#	)
	#endif ()
endif ()
