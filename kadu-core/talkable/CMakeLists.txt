set (talkable_SRCS
	filter/talkable-filter.cpp
	filter/unread-messages-talkable-filter.cpp

	model/talkable-model.cpp
	model/talkable-model-factory.cpp
	model/talkable-proxy-model.cpp

	talkable.cpp
)

set (talkable_MOC_SRCS
	filter/talkable-filter.h
	filter/unread-messages-talkable-filter.h

	model/talkable-model.h
	model/talkable-proxy-model.h
)

kadu_subdirectory (talkable "${talkable_SRCS}" "${talkable_MOC_SRCS}" "")
