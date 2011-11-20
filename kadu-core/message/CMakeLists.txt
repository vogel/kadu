set (message_SRCS
	formatted-message.cpp
	formatted-message-part.cpp
	message.cpp
	message-manager.cpp
	message-render-info.cpp
	message-shared.cpp
)

set (message_MOC_SRCS
	message-manager.h
	message-render-info.h
	message-shared.h
)

kadu_subdirectory (message "${message_SRCS}" "${message_MOC_SRCS}" "")
