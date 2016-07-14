set (misc_SRCS
	change-notifier.cpp
	change-notifier-lock.cpp
	date-time.cpp
	date-time-parser-tags.cpp
	error.cpp
	misc.cpp
	paths-provider.cpp
	string-utils.cpp
	syntax-list.cpp
)

set (misc_MOC_SRCS
	change-notifier.h
	syntax-list.h
)

kadu_add_test (graph/graph-find-cycles)
kadu_add_test (graph/graph-sort-successors)

kadu_add_test (find-overlaping-region)
kadu_add_test (sorted-unique-vector)
kadu_add_test (string-utils)

kadu_subdirectory (misc "${misc_SRCS}" "${misc_MOC_SRCS}" "")
