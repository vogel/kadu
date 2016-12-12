set (gui_SRCS
	configuration/chat-configuration-holder.cpp
	configuration/toolbar-configuration-manager.cpp

	services/clipboard-html-transformer-service.cpp

	gui-module.cpp
	hot-key.cpp
	status-icon.cpp
	web-view-highlighter.cpp
)

set (gui_MOC_SRCS
	services/clipboard-html-transformer-service.h

	hot-key.h
	scoped-updates-disabler.cpp
	status-icon.h
	web-view-highlighter.h
)

kadu_subdirectory (gui "${gui_SRCS}" "${gui_MOC_SRCS}" "")
