set (notify_SRCS
	account-notification.cpp
	buddy-notify-data.cpp
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
	buddy-notify-data.h
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

qt4_wrap_cpp (notify_MOC_FILES ${notify_MOC_SRCS})

add_library (notify
	STATIC
	${notify_SRCS}
	${notify_MOC_FILES}
)

set_target_properties (notify PROPERTIES COMPILE_DEFINITIONS KADULIB)
