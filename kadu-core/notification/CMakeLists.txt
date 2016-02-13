set (notification_SRCS
	listener/account-event-listener.cpp
	listener/chat-event-listener.cpp
	listener/group-event-listener.cpp

	notification-callback-repository.cpp
	notification-callback.cpp
	notification-configuration.cpp
	notification-dispatcher.cpp
	notification-event-repository.cpp
	notification-event.cpp
	notification-module.cpp
	notification-service.cpp
	notification.cpp
	notifier-configuration-data-manager.cpp
	notifier-repository.cpp
	notifier.cpp
	notify-configuration-importer.cpp
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
