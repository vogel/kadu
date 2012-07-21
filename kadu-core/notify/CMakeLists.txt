set (notify_SRCS
	account-event-listener.cpp
	account-notification.cpp
	chat-notification.cpp
	chat-event-listener.cpp
	event-listener.cpp
	group-event-listener.cpp
	new-message-notification.cpp
	multilogon-notification.cpp
	notification.cpp
	notification-manager.cpp
	notifier.cpp
	notify-configuration-ui-handler.cpp
	notify-event.cpp
	screen-mode-checker.cpp
	status-changed-notification.cpp
	window-notifier.cpp
)

set (notify_MOC_SRCS
	account-event-listener.h
	account-notification.h
	chat-notification.h
	chat-event-listener.h
	event-listener.h
	group-event-listener.h
	new-message-notification.h
	multilogon-notification.h
	notification.h
	notification-manager.h
	notifier.h
	notify-configuration-ui-handler.h
	screen-mode-checker.h
	status-changed-notification.h
	window-notifier.h
)

if (WIN32)
    list (APPEND notify_SRCS "windows-screen-mode-checker.cpp")
elseif (UNIX AND NOT APPLE)
    list (APPEND notify_SRCS "x11-screen-mode-checker.cpp")
endif()

kadu_subdirectory (notify "${notify_SRCS}" "${notify_MOC_SRCS}" "")
