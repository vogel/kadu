set (qt_SRCS
	long-validator.cpp
)

set (qt_MOC_SRCS
	long-validator.h
)

qt4_wrap_cpp (qt_MOC_FILES ${qt_MOC_SRCS})

add_library (qt
	STATIC
	${qt_SRCS}
	${qt_MOC_FILES}
)

set_target_properties (qt PROPERTIES COMPILE_DEFINITIONS KADULIB)
