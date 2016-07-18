set (chat_SRCS
	buddy-chat-manager.cpp
	chat.cpp
	chat-details-buddy.cpp
	chat-details-contact.cpp
	chat-details-contact-set.cpp
	chat-details-room.cpp
	chat-details.cpp
	chat-list-mime-data-service.cpp
	chat-manager-impl.cpp
	chat-manager.cpp
	chat-module.cpp
	chat-shared.cpp
	chat-storage.cpp
	open-chat-repository.cpp
	open-chat-service.cpp
	recent-chat-repository.cpp
	recent-chat-service.cpp

	model/chat-data-extractor.cpp
	model/chat-list-model.cpp
	model/chat-manager-adapter.cpp

	type/chat-type.cpp
	type/chat-type-buddy.cpp
	type/chat-type-contact.cpp
	type/chat-type-contact-set.cpp
	type/chat-type-manager.cpp
	type/chat-type-room.cpp
)

kadu_add_test (open-chat-repository)
kadu_add_test (open-chat-service)
kadu_add_test (recent-chat-repository)
kadu_add_test (recent-chat-service)

kadu_subdirectory (chat "${chat_SRCS}" "" "")
