set (status_SRCS
	all-accounts-status-container.cpp
	description-manager.cpp
	description-model.cpp
	status.cpp
	status-actions.cpp
	status-changer.cpp
	status-changer-manager.cpp
	status-configuration-holder.cpp
	status-container.cpp
	status-container-aware-object.cpp
	status-container-manager.cpp
	status-setter.cpp
	status-type-data.cpp
	status-type-manager.cpp
	storable-status-container.cpp
)

set (status_MOC_SRCS
	all-accounts-status-container.h
	description-manager.h
	description-model.h
	status-actions.h
	status-changer.h
	status-changer-manager.h
	status-container.h
	status-container-manager.h
	storable-status-container.h
)

kadu_subdirectory (status "${status_SRCS}" "${status_MOC_SRCS}" "")
