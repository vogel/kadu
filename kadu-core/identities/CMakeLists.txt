set (identities_SRCS
	model/identity-model.cpp
	identities-aware-object.cpp
	identity.cpp
	identity-manager.cpp
	identity-shared.cpp
)

set (identities_MOC_SRCS
	model/identity-model.h
	identity-manager.h
	identity-shared.h
)

qt_wrap_cpp (identities identities_MOC_FILES ${identities_MOC_SRCS})

add_library (identities
	STATIC
	${identities_SRCS}
	${identities_MOC_FILES}
)

set_target_properties (identities PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (identities PROPERTIES COMPILE_FLAGS "-DKADULIB")

