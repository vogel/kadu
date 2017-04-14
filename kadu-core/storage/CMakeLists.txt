set (storage_SRCS
	chat-list-storage.cpp
	custom-properties.cpp
	custom-properties-variant-wrapper.cpp
	named-storable-object.cpp
	shared.cpp
	storable-object.cpp
	storable-string-list.cpp
	storage-point-factory-impl.cpp
	storage-point-factory.cpp
	storage-point.cpp
	string-list-storage.cpp
	uuid-storable-object.cpp
)

kadu_subdirectory (storage "${storage_SRCS}" "" "")
