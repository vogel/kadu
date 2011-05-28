set (chat_SRCS
	aggregate-chat-manager.cpp
	chat.cpp
	chat-details-aggregate.cpp
	chat-details-conference.cpp
	chat-details-simple.cpp
	chat-geometry-data.cpp
	chat-manager.cpp
	chat-shared.cpp
	chat-styles-manager.cpp
	html-messages-renderer.cpp
	recent-chat-manager.cpp

	filter/chat-name-filter.cpp

	message/formatted-message.cpp
	message/formatted-message-part.cpp
	message/message.cpp
	message/message-render-info.cpp
	message/pending-messages-manager.cpp
	message/message-shared.cpp

	style-engines/chat-engine-adium/adium-style.cpp
	style-engines/chat-engine-adium/adium-time-formatter.cpp
	style-engines/chat-engine-adium/chat-engine-adium.cpp
	style-engines/chat-engine-kadu/chat-engine-kadu.cpp
	style-engines/chat-engine-kadu/kadu-chat-syntax.cpp

	type/chat-type.cpp
	type/chat-type-aware-object.cpp
	type/chat-type-conference.cpp
	type/chat-type-manager.cpp
	type/chat-type-simple.cpp
)

set (chat_MOC_SRCS
	chat-details.h
	chat-details-aggregate.h
	chat-details-conference.h
	chat-details-simple.h
	chat-geometry-data.h

	aggregate-chat-manager.h
	chat-manager.h
	chat-shared.h
	chat-styles-manager.h
	html-messages-renderer.h
	recent-chat-manager.h

	filter/chat-filter.h
	filter/chat-name-filter.h

	message/message-render-info.h
	message/pending-messages-manager.h
	message/message-shared.h

	style-engines/chat-engine-adium/chat-engine-adium.h
	style-engines/chat-engine-kadu/chat-engine-kadu.h

	type/chat-type.h
	type/chat-type-conference.h
	type/chat-type-manager.h
	type/chat-type-simple.h
)

qt4_wrap_cpp (chat_MOC_FILES ${chat_MOC_SRCS})

add_library (chat
	STATIC
	${chat_SRCS}
	${chat_MOC_FILES}
)

set_target_properties (chat PROPERTIES COMPILE_DEFINITIONS KADULIB)
