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
			set (inFiles ${inFiles} ${tmp})
		endforeach (ICON_THEME)

		if (inFiles)
			if (WIN32)
				set (guessed_paths
					"C:/Program Files/GnuWin32/bin"
					"C:/Program Files (x86)/GnuWin32/bin"
					"C:/Program Files/Git/bin"
					"C:/Program Files (x86)/Git/bin"
				)
			else (WIN32)
				set (guessed_paths)
			endif (WIN32)

			if (WIN32)
				find_program (CMD_EXECUTABLE cmd)
			endif (WIN32)
			find_program (GZIP_EXECUTABLE gzip
				PATHS ${guessed_paths}
				DOC "gzip command line utility"
			)
			if (NOT GZIP_EXECUTABLE OR (WIN32 AND NOT CMD_EXECUTABLE))
				if (WIN32 AND NOT CMD_EXECUTABLE)
					message (STATUS "Could not compress SVG files: cmd.exe not found")
				else (WIN32 AND NOT CMD_EXECUTABLE)
					message (STATUS "Could not compress SVG files: gzip not found")
				endif (WIN32 AND NOT CMD_EXECUTABLE)
			else (NOT GZIP_EXECUTABLE OR (WIN32 AND NOT CMD_EXECUTABLE))
				foreach (inFile ${inFiles})
					set (outFile "${CMAKE_CURRENT_BINARY_DIR}/svgzs/${inFile}z")
					if (WIN32)
						string (REGEX REPLACE "/[^/]*$" "" outFileDir "${outFile}")
						file (MAKE_DIRECTORY "${outFileDir}")
						set (command "${CMD_EXECUTABLE}" "/c" "\"\"${GZIP_EXECUTABLE}\" --best -c \"${CMAKE_CURRENT_SOURCE_DIR}/${inFile}\" > \"${outFile}\"\"")
					else (WIN32)
						set (command mkdir -p `dirname "${outFile}"` && "${GZIP_EXECUTABLE}" --best -c "${CMAKE_CURRENT_SOURCE_DIR}/${inFile}" > "${outFile}")
					endif (WIN32)

					add_custom_command (OUTPUT "${outFile}"
						COMMAND ${command}
					)

					set (outFiles ${outFiles} "${outFile}")
				endforeach (inFile)

				add_custom_target (compress_svgs ALL DEPENDS ${outFiles})
				add_dependencies (icon_themes compress_svgs)
			endif (NOT GZIP_EXECUTABLE OR (WIN32 AND NOT CMD_EXECUTABLE))
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
