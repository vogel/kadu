set (notify_SRCS
	listener/account-event-listener.cpp
	notification/account-notification.cpp
	notification/chat-notification.cpp
	listener/chat-event-listener.cpp
	listener/event-listener.cpp
	listener/group-event-listener.cpp
	notification/aggregate-notification.cpp
	notification/new-message-notification.cpp
	notification/multilogon-notification.cpp
	notification/notification.cpp
	notification-manager.cpp
	notifier.cpp
	notifier-configuration-data-manager.cpp
	notify-configuration-ui-handler.cpp
	notify-event.cpp
	screen-mode-checker.cpp
	notification/status-changed-notification.cpp
	window-notifier.cpp
)

set (notify_MOC_SRCS
	listener/account-event-listener.h
	notification/account-notification.h
	notification/chat-notification.h
	listener/chat-event-listener.h
	listener/event-listener.h
	listener/group-event-listener.h
	notification/aggregate-notification.h
	notification/new-message-notification.h
	notification/multilogon-notification.h
	notification/notification.h
	notification-manager.h
	notifier.h
	notify-configuration-ui-handler.h
	screen-mode-checker.h
	notification/status-changed-notification.h
	window-notifier.h
)

if (WIN32)
    list (APPEND notify_SRCS "windows-screen-mode-checker.cpp")
elseif (UNIX AND NOT APPLE)
    list (APPEND notify_SRCS "x11-screen-mode-checker.cpp")
endif ()

kadu_subdirectory (notify "${notify_SRCS}" "${notify_MOC_SRCS}" "")
