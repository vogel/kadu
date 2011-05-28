set (configuration_SRCS
	chat-configuration-holder.cpp
	config-file-data-manager.cpp
	configuration-aware-object.cpp
	configuration-file.cpp
	configuration-holder.cpp
	configuration-manager.cpp
	main-configuration-holder.cpp
	notifier-configuration-data-manager.cpp
	toolbar-configuration-manager.cpp
	xml-configuration-file.cpp
)

set (configuration_MOC_SRCS
	chat-configuration-holder.h
	config-file-data-manager.h
	configuration-holder.h
	configuration-window-data-manager.h
	main-configuration-holder.h
	notifier-configuration-data-manager.h
	toolbar-configuration-manager.h
)

qt4_wrap_cpp (configuration_MOC_FILES ${configuration_MOC_SRCS})

add_library (configuration
	STATIC
	${configuration_SRCS}
	${configuration_MOC_FILES}
)

set_target_properties (configuration PROPERTIES COMPILE_DEFINITIONS KADULIB)
