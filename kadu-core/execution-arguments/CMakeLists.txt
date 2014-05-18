set (execution-arguments_SRCS
	execution-arguments.cpp
	execution-arguments-builder.cpp
	execution-arguments-parser.cpp
)

kadu_subdirectory (execution-arguments "${execution-arguments_SRCS}" "" "")
