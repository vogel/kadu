set (accounts_SRCS
	account.cpp
	account-details.cpp
	account-manager.cpp
	account-proxy-settings.cpp
	account-shared.cpp
	accounts-aware-object.cpp

	filter/have-multilogon-filter.cpp
	filter/id-validity-filter.cpp
	filter/protocol-filter.cpp
	filter/writeable-contacts-list-filter.cpp

	model/accounts-model.cpp
	model/accounts-proxy-model.cpp
)

set (accounts_MOC_SRCS
	account-manager.h
	account-shared.h

	filter/abstract-account-filter.h
	filter/have-multilogon-filter.h
	filter/id-validity-filter.h
	filter/protocol-filter.h
	filter/writeable-contacts-list-filter.h

	model/accounts-model.h
	model/accounts-proxy-model.h
)

qt4_wrap_cpp (accounts_MOC_FILES ${accounts_MOC_SRCS})

add_library (accounts
	STATIC
	${accounts_SRCS}
	${accounts_MOC_FILES}
)

set_target_properties (accounts PROPERTIES COMPILE_DEFINITIONS KADULIB)
