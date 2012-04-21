set (protocols_SRCS
	filter/can-register-protocol-filter.cpp

	model/protocols-model.cpp
	model/protocols-model-proxy.cpp

	services/chat-image-service.cpp
	services/chat-service.cpp
	services/chat-state-service.cpp
	services/contact-list-service.cpp
	services/protocol-service.cpp
	services/roster-service.cpp

	services/roster/roster-entry.cpp
	services/roster/roster-task.cpp

	connection-error-notification.cpp
	protocol.cpp
	protocols-manager.cpp
	protocol-state-machine.cpp
	roster.cpp
)

set (protocols_MOC_SRCS
	filter/abstract-protocol-filter.h
	filter/can-register-protocol-filter.h

	model/protocols-model.h
	model/protocols-model-proxy.h

	services/avatar-service.h
	services/chat-image-service.h
	services/chat-service.h
	services/chat-state-service.h
	services/contact-list-service.h
	services/contact-personal-info-service.h
	services/file-transfer-service.h
	services/multilogon-service.h
	services/personal-info-service.h
	services/protocol-service.h
	services/roster-service.h
	services/search-service.h
	services/subscription-service.h

	services/roster/roster-entry.h

	connection-error-notification.h
	protocol.h
	protocol-factory.h
	protocols-manager.h
	protocol-state-machine.h
	roster.h
)

kadu_subdirectory (protocols "${protocols_SRCS}" "${protocols_MOC_SRCS}" "")
