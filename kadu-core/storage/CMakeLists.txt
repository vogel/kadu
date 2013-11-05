set (storage_SRCS
	chat-list-storage.cpp
	custom-properties.cpp
	custom-properties-variant-wrapper.cpp
	named-storable-object.cpp
	shared.cpp
	storable-object.cpp
	storable-string-list.cpp
	storage-point.cpp
	storage-point-factory.cpp
	string-list-storage.cpp
	uuid-storable-object.cpp
)

set (storage_MOC_SRCS
	custom-properties.h
)

kadu_subdirectory (storage "${storage_SRCS}" "${storage_MOC_SRCS}" "")
