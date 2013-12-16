set (plugins_SRCS
	dependency-graph/plugin-dependency-graph.cpp
	dependency-graph/plugin-dependency-graph-builder.cpp
	model/plugin-model.cpp
	model/plugin-proxy-model.cpp
	plugin.cpp
	plugin-activation-action.cpp
	plugin-activation-service.cpp
	plugin-info.cpp
	plugin-info-builder.cpp
	plugin-info-reader.cpp
	plugin-repository.cpp
	plugins-manager.cpp
)

set (plugins_MOC_SRCS
	model/plugin-model.h
	plugin.h
	plugins-manager.h
)

kadu_subdirectory (plugins "${plugins_SRCS}" "${plugins_MOC_SRCS}" "")
