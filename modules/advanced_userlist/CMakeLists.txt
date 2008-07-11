set (SOURCES
	advanced_userlist.cpp
	main.cpp
)

set (MOC_SOURCES
	advanced_userlist.h
)

set (TRANSLATION_SOURCES
	translations/advanced_userlist_de.ts
	translations/advanced_userlist_fr.ts
	translations/advanced_userlist_it.ts
	translations/advanced_userlist_pl.ts
)

set (CONFIGURATION_FILES
	configuration/advanced_userlist.ui)

kadu_module_desc (advanced_userlist.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (advanced_userlist MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (advanced_userlist ${advanced_userlist} ${SOURCES} ${MOC_FILES})
add_custom_target (advanced_userlist-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (advanced_userlist advanced_userlist-translations)

install (TARGETS advanced_userlist DESTINATION lib/kadu/modules)
