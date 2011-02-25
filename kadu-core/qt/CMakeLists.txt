set (qt_SRCS
	long-validator.cpp
)

set (qt_MOC_SRCS
	long-validator.h
)

qt_wrap_cpp (qt qt_MOC_FILES ${qt_MOC_SRCS})

add_library (qt
	STATIC
	${qt_SRCS}
	${qt_MOC_FILES}
)

set_target_properties (qt PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (qt PROPERTIES COMPILE_FLAGS "-DKADULIB")
