set (protocols_SRCS
	filter/abstract-protocol-filter.cpp
	filter/can-register-protocol-filter.cpp

	model/protocols-model.cpp
	model/protocols-model-proxy.cpp

	services/account-service.cpp
	services/buddy-list-serialization-service.cpp
	services/chat-image.cpp
	services/chat-image-service.cpp
	services/chat-service.cpp
	services/chat-state-service.cpp
	services/contact-personal-info-service.cpp
	services/file-transfer-service.cpp
	services/multilogon-service.cpp
	services/personal-info-service.cpp
	services/protocol-service.cpp
	services/search-service.cpp
	services/subscription-service.cpp

	protocol.cpp
	protocol-factory.cpp
	protocols-manager.cpp
	protocol-state-machine.cpp
)

kadu_subdirectory (protocols "${protocols_SRCS}" "" "")
