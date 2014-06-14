set (configuration_SRCS
	chat-configuration-holder.cpp
	config-file-data-manager.cpp
	config-file-variant-wrapper.cpp
	configuration.cpp
	configuration-api.cpp
	configuration-aware-object.cpp
	configuration-factory.cpp
	configuration-holder.cpp
	configuration-manager.cpp
	configuration-path-provider.cpp
	configuration-window-data-manager.cpp
	configuration-writer.cpp
	deprecated-configuration-api.cpp
	main-configuration-holder.cpp
	notifier-configuration-data-manager.cpp
	toolbar-configuration-manager.cpp
)

kadu_subdirectory (configuration "${configuration_SRCS}" "" "")
