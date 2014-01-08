set (plugin_SRCS
	dependency-graph/plugin-dependency-graph.cpp
	dependency-graph/plugin-dependency-graph-builder.cpp
	model/plugin-model.cpp
	model/plugin-proxy-model.cpp
	activation/active-plugin.cpp
	activation/plugin-activation-error-handler.cpp
	activation/plugin-activation-service.cpp
	activation/plugin-loader.cpp
	activation/plugin-root-component-handler.cpp
	activation/plugin-translations-loader.cpp
	plugin-info.cpp
	plugin-info-builder.cpp
	plugin-info-finder.cpp
	plugin-info-reader.cpp
	plugin-info-repository.cpp
	plugin-manager.cpp
	plugin-state-service.cpp
	plugin-state-storage.cpp
	plugin-state-storage-09.cpp
)

kadu_subdirectory (plugin "${plugin_SRCS}" "" "")
