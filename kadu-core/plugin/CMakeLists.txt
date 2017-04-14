set (plugin_SRCS
	activation/active-plugin.cpp
	activation/plugin-activation-error-handler.cpp
	activation/plugin-activation-service.cpp
	activation/plugin-injector-provider-container.cpp
	activation/plugin-loader.cpp
	activation/plugin-translations-loader.cpp

	dependency-graph/plugin-dependency-graph.cpp
	dependency-graph/plugin-dependency-graph-builder.cpp

	gui/plugin-error-dialog.cpp
	gui/plugin-list/plugin-list-widget.cpp
	gui/plugin-list/plugin-list-view-delegate.cpp
	gui/plugin-list/plugin-list-view-delegate-widgets.cpp
	gui/plugin-list/plugin-list-widget-item-delegate.cpp

	metadata/plugin-metadata-finder.cpp
	metadata/plugin-metadata-provider.cpp
	metadata/plugin-metadata-reader.cpp

	model/plugin-model.cpp
	model/plugin-proxy-model.cpp

	state/plugin-state.cpp
	state/plugin-state-manager.cpp
	state/plugin-state-service.cpp
	state/plugin-state-storage.cpp
	state/plugin-state-storage-09.cpp

	plugin-conflict-resolver.cpp
	plugin-dependency-handler.cpp
	plugin-injected-factory-module.cpp
	plugin-injected-factory.cpp
	plugin-injector-provider.cpp
	plugin-modules-factory.cpp
	plugin-manager.cpp
	plugin-module.cpp
)

kadu_add_test (dependency-graph/plugin-dependency-graph-builder)
kadu_add_test (dependency-graph/plugin-dependency-graph)

kadu_add_test (state/plugin-state-service)
kadu_add_test (state/plugin-state)

kadu_add_test (plugin-conflict-resolver)
kadu_add_test (plugin-dependency-handler)

kadu_subdirectory (plugin "${plugin_SRCS}")
