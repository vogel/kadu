set (services_SRCS
	image-storage-service.cpp
	notification-service.cpp
)

set (services_MOC_SRCS
	image-storage-service.h
	notification-service.h
)

kadu_subdirectory (services "${services_SRCS}" "${services_MOC_SRCS}" "")
