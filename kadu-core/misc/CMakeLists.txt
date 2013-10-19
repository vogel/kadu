set (misc_SRCS
	change-notifier.cpp
	change-notifier-lock.cpp
	date-time.cpp
	date-time-parser-tags.cpp
	error.cpp
	misc.cpp
	kadu-paths.cpp
	string-utils.cpp
	syntax-list.cpp
)

set (misc_MOC_SRCS
	change-notifier.h
	syntax-list.h
)

kadu_subdirectory (misc "${misc_SRCS}" "${misc_MOC_SRCS}" "")
