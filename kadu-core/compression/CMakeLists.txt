set (compression_SRCS
	archive-extractor.cpp
)

set (compression_MOC_SRCS
)

kadu_subdirectory (compression "${compression_SRCS}" "${compression_MOC_SRCS}" "")
