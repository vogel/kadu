set (storage_SRCS
	custom-properties.cpp
	named-storable-object.cpp
	shared.cpp
	storable-object.cpp
	storable-string-list.cpp
	storage-point.cpp
	uuid-storable-object.cpp
)

set (storage_MOC_SRCS
	custom-properties.h
)

kadu_subdirectory (storage "${storage_SRCS}" "${storage_MOC_SRCS}" "")
