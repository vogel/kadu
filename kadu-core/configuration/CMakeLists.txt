add_subdirectory (widgets)

set (configuration_SRCS
	config-file-data-manager.cpp
	configuration-window.cpp
	configuration-window-widgets.cpp
)

set (configuration_MOC_SRCS
	config-file-data-manager.h
	configuration-window.h
	configuration-window-data-manager.h
	configuration-window-widgets.h
)

qt_wrap_cpp (configuration MOC_FILES ${configuration_MOC_SRCS})
add_library (configuration STATIC ${configuration_SRCS} ${MOC_FILES})
set_target_properties(configuration PROPERTIES COMPILE_FLAGS "-DKADULIB" )

