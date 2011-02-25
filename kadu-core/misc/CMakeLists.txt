set (misc_SRCS
	coding-conversion.cpp
	date-time.cpp
	misc.cpp
	path-conversion.cpp
	syntax-list.cpp
)

set (misc_MOC_SRCS
	syntax-list.h
)

qt_wrap_cpp (misc misc_MOC_FILES ${misc_MOC_SRCS})

add_library (misc
	STATIC
	${misc_SRCS}
	${misc_MOC_FILES}
)

set_target_properties (misc PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (misc PROPERTIES COMPILE_FLAGS "-DKADULIB")
