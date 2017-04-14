set (services_SRCS
	chat-image-request-service.cpp
	chat-image-request-service-configuration.cpp
	chat-image-request-service-configurator.cpp
	image-storage-service-impl.cpp
	image-storage-service.cpp
	message-transformer-service.cpp
	raw-message-transformer-service.cpp
)

kadu_subdirectory (services "${services_SRCS}")
