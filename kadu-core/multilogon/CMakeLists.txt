set (multilogon_SRCS
	model/multilogon-model.cpp

	multilogon-session.cpp
)

set (multilogon_MOC_SRCS
	model/multilogon-model.h

	multilogon-session.h
)

kadu_subdirectory (multilogon "${multilogon_SRCS}" "${multilogon_MOC_SRCS}" "")
