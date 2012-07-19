set (services_SRCS
	chat-image-request-service.cpp
	chat-image-request-service-configuration.cpp
	chat-image-request-service-configurator.cpp
	image-storage-service.cpp
	notification-service.cpp
)

set (services_MOC_SRCS
	chat-image-request-service.h
	image-storage-service.h
	notification-service.h
)

kadu_subdirectory (services "${services_SRCS}" "${services_MOC_SRCS}" "")
