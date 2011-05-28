set (icons_SRCS
	icons-manager.cpp
	kadu-icon.cpp
)

set (icons_MOC_SRCS
	icons-manager.h
	kadu-icon.h
)

qt4_wrap_cpp (icons_MOC_FILES ${icons_MOC_SRCS})

add_library (icons
	STATIC
	${icons_SRCS}
	${icons_MOC_FILES}
)

set_target_properties (icons PROPERTIES COMPILE_DEFINITIONS KADULIB)
