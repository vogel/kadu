set (accounts_SRCS
	account.cpp
	account-manager.cpp
	account-module.cpp
	account-notification-service.cpp
	account-shared.cpp
	account-status-container.cpp
	account-storage.cpp
	accounts-aware-object.cpp

	filter/abstract-account-filter.cpp
	filter/have-multilogon-filter.cpp
	filter/have-protocol-filter.cpp
	filter/protocol-filter.cpp
	filter/writeable-contacts-list-filter.cpp

	model/accounts-model.cpp
	model/accounts-proxy-model.cpp
)

kadu_subdirectory (accounts "${accounts_SRCS}" "" "")
