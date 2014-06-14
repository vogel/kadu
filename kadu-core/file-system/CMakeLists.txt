set (file-system_SRCS
	atomic-file-writer.cpp
)

kadu_subdirectory (file-system "${file-system_SRCS}" "" "")
