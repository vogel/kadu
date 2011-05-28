set (parser_SRCS
	parser.cpp
)

add_library (parser
	STATIC
	${parser_SRCS}
)

set_target_properties (parser PROPERTIES COMPILE_DEFINITIONS KADULIB)
