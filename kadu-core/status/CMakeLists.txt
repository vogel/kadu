set (status_SRCS
	all-accounts-status-container.cpp
	base-status-container.cpp
	description-manager.cpp
	description-model.cpp
	status.cpp
	status-actions.cpp
	status-changer.cpp
	status-changer-manager.cpp
	status-container-aware-object.cpp
	status-container-manager.cpp
	status-setter.cpp
	status-type.cpp
	status-type-manager.cpp
)

set (status_MOC_SRCS
	all-accounts-status-container.h
	base-status-container.h
	description-manager.h
	description-model.h
	status-actions.h
	status-changer.h
	status-changer-manager.h
	status-container.h
	status-container-manager.h
)

kadu_subdirectory (status "${status_SRCS}" "${status_MOC_SRCS}" "")
