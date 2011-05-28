set (plugins_SRCS
	plugin.cpp
	plugin-info.cpp
	plugins-manager.cpp
)

set (plugins_MOC_SRCS
	plugin.h
	plugins-manager.h
)

qt4_wrap_cpp (plugins_MOC_FILES ${plugins_MOC_SRCS})

add_library (plugins
	STATIC
	${plugins_SRCS}
	${plugins_MOC_FILES}
)

set_target_properties (plugins PROPERTIES COMPILE_DEFINITIONS KADULIB)
