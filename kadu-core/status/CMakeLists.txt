set (status_SRCS
	all-accounts-status-container.cpp
	description-manager.cpp
	description-model.cpp
	remote-status-request-handler.cpp
	status.cpp
	status-actions.cpp
	status-changer.cpp
	status-changer-manager.cpp
	status-configuration-holder.cpp
	status-container.cpp
	status-container-aware-object.cpp
	status-container-manager.cpp
	status-module.cpp
	status-notification-service.cpp
	status-setter.cpp
	status-type-data.cpp
	status-type-manager.cpp
	storable-status-container.cpp
)

kadu_subdirectory (status "${status_SRCS}")
