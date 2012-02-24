set (notify_SRCS
	account-notification.cpp
	chat-notification.cpp
	new-message-notification.cpp
	multilogon-notification.cpp
	notification.cpp
	notification-manager.cpp
	notifier.cpp
	notify-configuration-ui-handler.cpp
	notify-event.cpp
	status-changed-notification.cpp
	window-notifier.cpp
)

set (notify_MOC_SRCS
	account-notification.h
	chat-notification.h
	new-message-notification.h
	multilogon-notification.h
	notification.h
	notification-manager.h
	notifier.h
	notify-configuration-ui-handler.h
	status-changed-notification.h
	window-notifier.h
)

kadu_subdirectory (notify "${notify_SRCS}" "${notify_MOC_SRCS}" "")
