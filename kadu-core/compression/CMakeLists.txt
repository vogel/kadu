set (compression_SRCS
	archive-extractor.cpp
)

set (compression_MOC_SRCS
)

if (NOT MSVC)
	set_property (SOURCE archive-extractor.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-error")
endif ()

kadu_subdirectory (compression "${compression_SRCS}" "${compression_MOC_SRCS}" "")
