set (message_SRCS
	message.cpp
	message-html-renderer-service.cpp
	message-manager.cpp
	message-render-info.cpp
	message-render-info-builder.cpp
	message-render-info-factory.cpp
	message-shared.cpp
	raw-message.cpp
	sorted-messages.cpp
	unread-message-repository.cpp
)

set (message_MOC_SRCS
	message-html-renderer-service.h
	message-manager.h
	message-shared.h
)

kadu_subdirectory (message "${message_SRCS}" "${message_MOC_SRCS}" "")
