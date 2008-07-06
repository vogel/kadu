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

kadu_module_desc (account_management.desc)
qt_wrap_cpp (account_management MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (account_management ${account_management} ${SOURCES} ${MOC_FILES})
add_custom_target (account_management-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (account_management account_management-translations)

install (TARGETS account_management DESTINATION lib/kadu/modules)
