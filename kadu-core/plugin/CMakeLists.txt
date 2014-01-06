set (plugin_SRCS
	dependency-graph/plugin-dependency-graph.cpp
	dependency-graph/plugin-dependency-graph-builder.cpp
	model/plugin-model.cpp
	model/plugin-proxy-model.cpp
	active-plugin.cpp
	plugin-activation-error-handler.cpp
	plugin-activation-service.cpp
	plugin-info.cpp
	plugin-info-builder.cpp
	plugin-info-finder.cpp
	plugin-info-reader.cpp
	plugin-info-repository.cpp
	plugin-loader.cpp
	plugin-manager.cpp
	plugin-root-component-handler.cpp
	plugin-state-service.cpp
	plugin-state-storage.cpp
	plugin-state-storage-09.cpp
	plugin-translations-loader.cpp
)

kadu_subdirectory (plugin "${plugin_SRCS}" "${plugin_MOC_SRCS}" "")
