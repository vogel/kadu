set (plugins_SRCS
	plugin.cpp
	plugin-info.cpp
	plugins-manager.cpp
)

set (plugins_MOC_SRCS
	plugin.h
	plugins-manager.h
)

kadu_subdirectory (plugins "${plugins_SRCS}" "${plugins_MOC_SRCS}" "")
