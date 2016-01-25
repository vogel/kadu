set (themes_SRCS
	icon-theme-manager.cpp
	themes-module.cpp
	theme-manager.cpp
	theme.cpp
)

set (themes_MOC_SRCS
	icon-theme-manager.h
	theme-manager.h
)

kadu_subdirectory (themes "${themes_SRCS}" "${themes_MOC_SRCS}" "")
