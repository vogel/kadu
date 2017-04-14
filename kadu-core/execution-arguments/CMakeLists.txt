set (execution-arguments_SRCS
	execution-arguments.cpp
	execution-arguments-builder.cpp
	execution-arguments-parser.cpp
)

kadu_add_test (execution-arguments-parser)

kadu_subdirectory (execution-arguments "${execution-arguments_SRCS}")
