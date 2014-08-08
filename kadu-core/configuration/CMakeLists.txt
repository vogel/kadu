set (configuration_SRCS
	config-file-data-manager.cpp
	config-file-variant-wrapper.cpp
	configuration.cpp
	configuration-api.cpp
	configuration-aware-object.cpp
	configuration-factory.cpp
	configuration-holder.cpp
	configuration-manager.cpp
	configuration-module.cpp
	configuration-path-provider.cpp
	configuration-window-data-manager.cpp
	configuration-writer.cpp
	deprecated-configuration-api.cpp
)

kadu_subdirectory (configuration "${configuration_SRCS}" "" "")
