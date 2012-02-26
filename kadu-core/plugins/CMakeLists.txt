set (plugins_SRCS
	model/plugin-model.cpp
	model/plugin-proxy-model.cpp
	plugin.cpp
	plugin-info.cpp
	plugins-manager.cpp
)

set (plugins_MOC_SRCS
	model/plugin-model.h
	plugin.h
	plugins-manager.h
)
kadu_subdirectory (plugins "${plugins_SRCS}" "${plugins_MOC_SRCS}" "")
