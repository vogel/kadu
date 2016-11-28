file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" KADU_VERSION LIMIT_COUNT 1)

find_package (Git QUIET)
if (GIT_FOUND)
	execute_process (COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE git_tag
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (NOT (result STREQUAL 0) OR NOT (git_tag STREQUAL KADU_VERSION))
		execute_process (COMMAND "${GIT_EXECUTABLE}" rev-parse --short --verify HEAD
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			RESULT_VARIABLE result
			OUTPUT_VARIABLE git_commit
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		if (result STREQUAL 0)
			set (KADU_VERSION "${KADU_VERSION}-g${git_commit}")
		endif ()
	endif ()
endif ()

string (REGEX REPLACE "-.*" "" simple_version ${KADU_VERSION})

if (NOT KADU_VERSION STREQUAL simple_version)
	set (KADU_PRERELEASE TRUE)
endif ()
