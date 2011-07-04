set (os_SRCS
	generic/compositing-aware-object.cpp
	generic/system-info.cpp
	generic/url-opener.cpp
)

set (os_MOC_SRCS
	qtsingleapplication/qtlocalpeer.h
)

if (MINGW)
	set_source_files_properties (generic/system-info.cpp PROPERTIES
		COMPILE_FLAGS -fno-strict-aliasing
	)
endif (MINGW)

qt4_wrap_cpp (os_MOC_FILES ${os_MOC_SRCS})

add_library (os
	STATIC
	${os_SRCS}
	${os_MOC_FILES}
)

set_target_properties (os PROPERTIES COMPILE_DEFINITIONS KADULIB)
