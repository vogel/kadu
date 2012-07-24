set (message_SRCS
	message.cpp
	message-filter.cpp
	message-manager.cpp
	message-render-info.cpp
	message-shared.cpp
	message-transformer.cpp
)

set (message_MOC_SRCS
	message-filter.h
	message-manager.h
	message-render-info.h
	message-shared.h
	message-transformer.h
)

kadu_subdirectory (message "${message_SRCS}" "${message_MOC_SRCS}" "")
