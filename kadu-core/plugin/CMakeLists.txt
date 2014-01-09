set (plugin_SRCS
	activation/active-plugin.cpp
	activation/plugin-activation-error-handler.cpp
	activation/plugin-activation-service.cpp
	activation/plugin-loader.cpp
	activation/plugin-root-component-handler.cpp
	activation/plugin-translations-loader.cpp
	dependency-graph/plugin-dependency-graph.cpp
	dependency-graph/plugin-dependency-graph-builder.cpp
	model/plugin-model.cpp
	model/plugin-proxy-model.cpp
	state/plugin-state-service.cpp
	state/plugin-state-storage.cpp
	state/plugin-state-storage-09.cpp
	plugin-manager.cpp
	plugin-metadata.cpp
	plugin-metadata-builder.cpp
	plugin-metadata-finder.cpp
	plugin-metadata-reader.cpp
	plugin-metadata-repository.cpp
)

kadu_subdirectory (plugin "${plugin_SRCS}" "" "")
