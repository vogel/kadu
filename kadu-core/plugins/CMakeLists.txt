set (plugins_SRCS
	plugin-info.cpp
)

add_library (plugins
	STATIC
	${plugins_SRCS}
)

set_target_properties (plugins PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (plugins PROPERTIES COMPILE_FLAGS "-DKADULIB")
