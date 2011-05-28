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

qt4_wrap_cpp (themes_MOC_FILES ${themes_MOC_SRCS})

add_library (themes
	STATIC
	${themes_SRCS}
	${themes_MOC_FILES}
)

set_target_properties (themes PROPERTIES COMPILE_DEFINITIONS KADULIB)
