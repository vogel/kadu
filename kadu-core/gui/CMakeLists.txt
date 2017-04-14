set (gui_SRCS
	configuration/chat-configuration-holder.cpp
	configuration/toolbar-configuration-manager.cpp

	services/clipboard-html-transformer-service.cpp

	gui-module.cpp
	hot-key.cpp
	scoped-updates-disabler.cpp
	status-icon.cpp
	web-view-highlighter.cpp
)

kadu_subdirectory (gui "${gui_SRCS}")
