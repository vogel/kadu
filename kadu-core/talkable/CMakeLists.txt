set (talkable_SRCS
	model/talkable-proxy-model.cpp

	talkable.cpp
)

set (talkable_MOC_SRCS
	model/talkable-proxy-model.h
)

kadu_subdirectory (talkable "${talkable_SRCS}" "${talkable_MOC_SRCS}" "")
