set (ICON_THEMES
	default
	glass
	oxygen
)

if (ICON_THEMES)
	add_custom_target (icon_themes
		DEPENDS ${ICON_THEMES}
	)
	add_dependencies (kadu icon_themes)
	
	if (UNIX AND NOT APPLE)
		option (COMPRESS_SVG "Compress SVG files when installing (will break freedesktop_notify on notification-daemon, notify-osd and possibly other daemons, though will work on KDE)" OFF)
	else (UNIX AND NOT APPLE)
		option (COMPRESS_SVG "Compress SVG files when installing" ON)
	endif (UNIX AND NOT APPLE)

	if (COMPRESS_SVG)
		message (STATUS "SVG icons compressing enabled")

		foreach (ICON_THEME ${ICON_THEMES})
			file (GLOB_RECURSE tmp RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
				"${CMAKE_CURRENT_SOURCE_DIR}/${ICON_THEME}/*.svg"
			)
			list (APPEND inFiles ${tmp})
		endforeach (ICON_THEME)

		if (inFiles)
			if (WIN32)
				set (guessed_paths
					"C:/Program Files/Git/bin"
					"C:/Program Files (x86)/Git/bin"
					"C:/Program Files/GnuWin32/bin"
					"C:/Program Files (x86)/GnuWin32/bin"
				)
			endif (WIN32)

			find_program (GZIP_EXECUTABLE gzip
				PATHS ${guessed_paths}
				DOC "gzip command line utility"
			)
			if (NOT GZIP_EXECUTABLE)
				message (STATUS "Could not compress SVG files: gzip not found")
			else (NOT GZIP_EXECUTABLE)
				foreach (inFile ${inFiles})
					set (outFile "${CMAKE_CURRENT_BINARY_DIR}/svgzs/${inFile}z")
					list (APPEND outFiles "${outFile}")
					string (REGEX REPLACE "/[^/]*$" "" outFileDir "${outFile}")

					add_custom_command (OUTPUT "${outFile}"
						COMMAND "${CMAKE_COMMAND}" -E make_directory "${outFileDir}"
						COMMAND "${GZIP_EXECUTABLE}" --best -c "${CMAKE_CURRENT_SOURCE_DIR}/${inFile}" > "${outFile}"
						VERBATIM
					)
				endforeach (inFile)

				add_custom_target (compress_svgs ALL DEPENDS ${outFiles})
				add_dependencies (icon_themes compress_svgs)
			endif (NOT GZIP_EXECUTABLE)
		endif (inFiles)
	else (COMPRESS_SVG)
		message (STATUS "SVG icons compressing disabled")
	endif (COMPRESS_SVG)

	foreach (ICON_THEME ${ICON_THEMES})
		message (STATUS "Icon theme: " ${ICON_THEME})
	endforeach (ICON_THEME)

	install (DIRECTORY ${ICON_THEMES}
		DESTINATION "${KADU_DATADIR}/themes/icons"
		FILES_MATCHING
		PATTERN "*.gif"
		PATTERN "*.png"
		PATTERN "*.svgz"
		# CMake has no option to exclude empty dirs (empty because of pattern matching above)
		PATTERN "src" EXCLUDE
	)

	if (TARGET compress_svgs)
		install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/svgzs/"
			DESTINATION "${KADU_DATADIR}/themes/icons"
			FILES_MATCHING
			PATTERN "*.svgz"
		)
	else (TARGET compress_svgs)
		install (DIRECTORY ${ICON_THEMES}
			DESTINATION "${KADU_DATADIR}/themes/icons"
			FILES_MATCHING
			PATTERN "*.svg"
			PATTERN "src" EXCLUDE
		)
	endif (TARGET compress_svgs)
endif (ICON_THEMES)
