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

kadu_subdirectory (identities "${identities_SRCS}" "${identities_MOC_SRCS}" "")
