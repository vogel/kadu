set (services_SRCS
	chat-image-request-service.cpp
	chat-image-request-service-configuration.cpp
	chat-image-request-service-configurator.cpp
	image-storage-service.cpp
	message-filter-service.cpp
	message-transformer-service.cpp
	notification-service.cpp
	raw-message-transformer-service.cpp
)

set (services_MOC_SRCS
	chat-image-request-service.h
	image-storage-service.h
	message-filter-service.h
	message-transformer-service.h
	notification-service.h
	raw-message-transformer-service.h
)

kadu_subdirectory (services "${services_SRCS}" "${services_MOC_SRCS}" "")
