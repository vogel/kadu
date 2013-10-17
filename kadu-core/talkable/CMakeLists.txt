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
	model/talkable-model-factory.cpp
	model/talkable-proxy-model.cpp

	talkable.cpp
)

set (talkable_MOC_SRCS
	filter/account-talkable-filter.h
	filter/blocked-talkable-filter.h
	filter/exclude-buddy-talkable-filter.h
	filter/group-talkable-filter.h
	filter/hide-anonymous-talkable-filter.h
	filter/hide-contact-chats-talkable-filter.h
	filter/hide-offline-talkable-filter.h
	filter/hide-offline-without-description-talkable-filter.h
	filter/hide-without-description-talkable-filter.h
	filter/hide-temporary-talkable-filter.h
	filter/mobile-talkable-filter.h
	filter/name-talkable-filter.h
	filter/talkable-filter.h
	filter/unread-messages-talkable-filter.h

	model/talkable-model.h
	model/talkable-proxy-model.h
)

kadu_subdirectory (talkable "${talkable_SRCS}" "${talkable_MOC_SRCS}" "")
