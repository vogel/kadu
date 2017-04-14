set (message_SRCS
	message.cpp
	message-filter-service.cpp
	message-html-renderer-service.cpp
	message-manager.cpp
	message-manager-impl.cpp
	message-module.cpp
	message-notification-service.cpp
	message-render-info.cpp
	message-render-info-builder.cpp
	message-render-info-factory.cpp
	message-shared.cpp
	message-storage.cpp
	raw-message.cpp
	sorted-messages.cpp
	unread-message-repository.cpp
)

kadu_add_test (message-html-renderer-service)

kadu_subdirectory (message "${message_SRCS}")
