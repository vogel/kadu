set (identities_SRCS
	model/identity-model.cpp
	identities-aware-object.cpp
	identity-manager.cpp
	identity-module.cpp
	identity-shared.cpp
	identity-status-container.cpp
	identity-storage.cpp
	identity.cpp
)

kadu_subdirectory (identities "${identities_SRCS}")
