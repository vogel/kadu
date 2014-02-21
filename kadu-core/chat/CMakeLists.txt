set (chat_SRCS
	buddy-chat-manager.cpp
	chat.cpp
	chat-details-buddy.cpp
	chat-details-contact.cpp
	chat-details-contact-set.cpp
	chat-details-room.cpp
	chat-list-mime-data-helper.cpp
	chat-manager.cpp
	chat-shared.cpp
	recent-chat-manager.cpp

	model/chat-data-extractor.cpp
	model/chat-list-model.cpp
	model/chat-manager-adapter.cpp

	type/chat-type.cpp
	type/chat-type-aware-object.cpp
	type/chat-type-buddy.cpp
	type/chat-type-contact.cpp
	type/chat-type-contact-set.cpp
	type/chat-type-manager.cpp
	type/chat-type-room.cpp
)

set (chat_MOC_SRCS
	buddy-chat-manager.h
	chat-details.h
	chat-details-buddy.h
	chat-details-contact.h
	chat-details-contact-set.h
	chat-details-room.h
	chat-manager.h
	chat-shared.h
	recent-chat-manager.h

	model/chat-list-model.h
	model/chat-manager-adapter.h

	type/chat-type.h
	type/chat-type-buddy.h
	type/chat-type-contact.h
	type/chat-type-contact-set.h
	type/chat-type-manager.h
	type/chat-type-room.h
)

kadu_subdirectory (chat "${chat_SRCS}" "${chat_MOC_SRCS}" "")
