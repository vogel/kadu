add_subdirectory (widgets)

set (configuration_SRCS
	configuration-window.cpp
	configuration-window-widgets.cpp
)

set (configuration_MOC_SRCS
	configuration-window.h
	configuration-window-widgets.h
)

qt_wrap_cpp (configuration MOC_FILES ${configuration_MOC_SRCS})
add_library (configuration STATIC ${configuration_SRCS} ${MOC_FILES})
