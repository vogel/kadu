set (multilogon_SRCS
	model/multilogon-model.cpp
	multilogon-session.cpp
)

kadu_subdirectory (multilogon "${multilogon_SRCS}" "" "")
