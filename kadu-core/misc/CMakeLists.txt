set (misc_SRCS
	coding-conversion.cpp
	date-time.cpp
	date-time-parser-tags.cpp
	misc.cpp
	path-conversion.cpp
	syntax-list.cpp
)

set (misc_MOC_SRCS
	syntax-list.h
)

kadu_subdirectory (misc "${misc_SRCS}" "${misc_MOC_SRCS}" "")
