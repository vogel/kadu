set (chat_SRCS
	aggregate-chat-manager.cpp
	chat.cpp
	chat-details-aggregate.cpp
	chat-details-conference.cpp
	chat-details-simple.cpp
	chat-geometry-data.cpp
	chat-list-mime-data-helper.cpp
	chat-manager.cpp
	chat-shared.cpp
	chat-styles-manager.cpp
	html-messages-renderer.cpp
	recent-chat-manager.cpp

	model/chat-data-extractor.cpp
	model/chats-list-model.cpp
	model/chats-model.cpp

	style-engines/chat-engine-adium/adium-style.cpp
	style-engines/chat-engine-adium/adium-time-formatter.cpp
	style-engines/chat-engine-adium/chat-engine-adium.cpp
	style-engines/chat-engine-kadu/chat-engine-kadu.cpp
	style-engines/chat-engine-kadu/kadu-chat-syntax.cpp

	type/chat-type.cpp
	type/chat-type-aggregate.cpp
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

	model/chats-list-model.h
	model/chats-model.h

	style-engines/chat-engine-adium/chat-engine-adium.h
	style-engines/chat-engine-kadu/chat-engine-kadu.h

	type/chat-type.h
	type/chat-type-aggregate.h
	type/chat-type-conference.h
	type/chat-type-manager.h
	type/chat-type-simple.h
)

kadu_subdirectory (chat "${chat_SRCS}" "${chat_MOC_SRCS}" "")
