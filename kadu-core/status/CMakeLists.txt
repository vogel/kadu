set (status_SRCS
	base-status-container.cpp
	description-manager.cpp
	description-model.cpp
	status.cpp
	status-actions.cpp
	status-changer.cpp
	status-changer-manager.cpp
	status-container-aware-object.cpp
	status-container-manager.cpp
	status-group.cpp
	status-group-manager.cpp
	status-type.cpp
	status-type-manager.cpp
)

set (status_MOC_SRCS
	base-status-container.h
	description-manager.h
	description-model.h
	status-actions.h
	status-changer.h
	status-changer-manager.h
	status-container.h
	status-container-manager.h
)

qt4_wrap_cpp (status_MOC_FILES ${status_MOC_SRCS})

add_library (status
	STATIC
	${status_SRCS}
	${status_MOC_FILES}
)

set_target_properties (status PROPERTIES DEFINE_SYMBOL KADULIB)
