set (storage_SRCS
	named-storable-object.cpp
	module-data.cpp
	shared.cpp
	storable-object.cpp
	storable-string-list.cpp
	storage-point.cpp
	uuid-storable-object.cpp
)

set (storage_MOC_SRCS
	module-data.h
)

kadu_subdirectory (storage "${storage_SRCS}" "${storage_MOC_SRCS}" "")
