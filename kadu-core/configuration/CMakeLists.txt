set (configuration_SRCS
	config-file-data-manager.cpp
	configuration-aware-object.cpp
	configuration-file.cpp
	configuration-manager.cpp
	main-configuration.cpp
	notifier-configuration-data-manager.cpp
	toolbar-configuration-manager.cpp
	xml-configuration-file.cpp
)

set (configuration_MOC_SRCS
	config-file-data-manager.h
	configuration-window-data-manager.h
	main-configuration.h
	notifier-configuration-data-manager.h
	toolbar-configuration-manager.h
)

qt4_wrap_cpp (configuration_MOC_FILES ${configuration_MOC_SRCS})

add_library (configuration
	STATIC
	${configuration_SRCS}
	${configuration_MOC_FILES}
)

set_target_properties (configuration PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (configuration PROPERTIES COMPILE_FLAGS "-DKADULIB")
