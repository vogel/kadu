set (accounts_SRCS
	account.cpp
	account-details.cpp
	account-manager.cpp
	account-shared.cpp
	account-status-container.cpp
	accounts-aware-object.cpp

	filter/have-multilogon-filter.cpp
	filter/protocol-filter.cpp
	filter/writeable-contacts-list-filter.cpp

	model/accounts-model.cpp
	model/accounts-proxy-model.cpp
)

set (accounts_MOC_SRCS
	account-manager.h
	account-shared.h
	account-status-container.h

	filter/abstract-account-filter.h
	filter/have-multilogon-filter.h
	filter/protocol-filter.h
	filter/writeable-contacts-list-filter.h

	model/accounts-model.h
	model/accounts-proxy-model.h
)

kadu_subdirectory (accounts "${accounts_SRCS}" "${accounts_MOC_SRCS}" "")
