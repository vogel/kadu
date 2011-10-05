set (themes_SRCS
	emoticon-theme-manager.cpp
	icon-theme-manager.cpp
	theme.cpp
	theme-manager.cpp
)

set (themes_MOC_SRCS
	emoticon-theme-manager.h
	icon-theme-manager.h
	theme-manager.h
)

kadu_subdirectory (themes "${themes_SRCS}" "${themes_MOC_SRCS}" "")
