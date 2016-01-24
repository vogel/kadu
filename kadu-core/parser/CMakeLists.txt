set (parser_SRCS
	parser.cpp
	parser-module.cpp
	parser-token.cpp
)

kadu_subdirectory (parser "${parser_SRCS}" "" "")
