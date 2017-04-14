set (talkable_SRCS
	filter/account-talkable-filter.cpp
	filter/blocked-talkable-filter.cpp
	filter/exclude-buddy-talkable-filter.cpp
	filter/group-filter.cpp
	filter/group-talkable-filter.cpp
	filter/hide-anonymous-talkable-filter.cpp
	filter/hide-contact-chats-talkable-filter.cpp
	filter/hide-offline-talkable-filter.cpp
	filter/hide-offline-without-description-talkable-filter.cpp
	filter/hide-without-description-talkable-filter.cpp
	filter/hide-temporary-talkable-filter.cpp
	filter/mobile-talkable-filter.cpp
	filter/name-talkable-filter.cpp
	filter/talkable-filter.cpp
	filter/unread-messages-talkable-filter.cpp

	model/talkable-model.cpp
	model/talkable-proxy-model.cpp

	talkable-converter.cpp
	talkable-module.cpp
	talkable.cpp
)

kadu_subdirectory (talkable "${talkable_SRCS}" "" "")
