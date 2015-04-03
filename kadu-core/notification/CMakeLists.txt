set (notification_SRCS
	listener/account-event-listener.cpp
	listener/chat-event-listener.cpp
	listener/event-listener.cpp
	listener/group-event-listener.cpp

	notification/aggregate-notification.cpp
	notification/new-message-notification.cpp
	notification/multilogon-notification.cpp
	notification/notification.cpp
	notification/status-changed-notification.cpp

	notification-callback-repository.cpp
	notification-callback.cpp
	notification-event-repository.cpp
	notification-event.cpp
	notification-manager.cpp
	notification-module.cpp
	notification-service.cpp
	notifier.cpp
	notifier-configuration-data-manager.cpp
	notify-configuration-ui-handler.cpp
	screen-mode-checker.cpp
	window-notifier.cpp
)

if (WIN32)
    list (APPEND notification_SRCS "windows-screen-mode-checker.cpp")
elseif (UNIX AND NOT APPLE)
    list (APPEND notification_SRCS "x11-screen-mode-checker.cpp")
endif ()

kadu_subdirectory (notification "${notification_SRCS}" "" "")
