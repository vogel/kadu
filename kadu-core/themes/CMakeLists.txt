set (themes_SRCS
	icon-theme-manager.cpp
	theme.cpp
	theme-manager.cpp
)

set (themes_MOC_SRCS
	icon-theme-manager.h
	theme-manager.h
)

kadu_subdirectory (themes "${themes_SRCS}" "${themes_MOC_SRCS}" "")
