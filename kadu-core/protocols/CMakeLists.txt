set (protocols_SRCS
	protocol.cpp
	protocols_manager.cpp
)

set (protocols_MOC_SRCS
	protocol.h
	protocols_manager.h
)

qt_wrap_cpp (protocols MOC_FILES ${protocols_MOC_SRCS})
add_library (protocols STATIC ${protocols_SRCS} ${MOC_FILES})

set_target_properties(protocols PROPERTIES COMPILE_FLAGS "-DKADULIB" )

