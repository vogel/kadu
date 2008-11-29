set (configuration_widgets_SRCS
	manage-accounts.cpp
)

set (configuration_widgets_MOC_SRCS
	manage-accounts.h
)

qt_wrap_cpp (configuration_widgets MOC_FILES ${configuration_widgets_MOC_SRCS})
add_library (configuration_widgets STATIC ${configuration_widgets_SRCS} ${MOC_FILES})
