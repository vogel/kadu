set (os_SRCS
	generic/compositing-aware-object.cpp
	generic/desktop-aware-object.cpp
	generic/desktop-aware-object-helper.cpp
	generic/system-info.cpp
	generic/url-opener.cpp
)

set (os_MOC_SRCS
	generic/desktop-aware-object-helper.h
	qtsingleapplication/qtlocalpeer.h
)

if (MINGW)
	set_source_files_properties (generic/system-info.cpp PROPERTIES
		COMPILE_FLAGS -fno-strict-aliasing
	)
endif (MINGW)

kadu_subdirectory (os "${os_SRCS}" "${os_MOC_SRCS}" "")
