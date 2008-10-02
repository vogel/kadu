set (SOURCES
	account_management.cpp
	change_password.cpp
	register.cpp
	remind_password.cpp
	unregister.cpp
)

set (MOC_SOURCES
	account_management.h
	change_password.h
	register.h
	remind_password.h
	unregister.h
)

set (TRANSLATION_SOURCES
	translations/account_management_de.ts
	translations/account_management_fr.ts
	translations/account_management_it.ts
	translations/account_management_pl.ts
)

kadu_module (account_management
	"${SOURCES}" "${MOC_SOURCES}"
	"${TRANSLATION_SOURCES}"
)
