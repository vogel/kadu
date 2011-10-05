set (icons_SRCS
	icons-manager.cpp
	kadu-icon.cpp
)

set (icons_MOC_SRCS
	icons-manager.h
	kadu-icon.h
)

kadu_subdirectory (icons "${icons_SRCS}" "${icons_MOC_SRCS}" "")
