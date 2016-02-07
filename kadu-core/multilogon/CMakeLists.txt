set (multilogon_SRCS
	model/multilogon-model.cpp
	multilogon-module.cpp
	multilogon-notification-service.cpp
	multilogon-session.cpp
)

kadu_subdirectory (multilogon "${multilogon_SRCS}" "" "")
