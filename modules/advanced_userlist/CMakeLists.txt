set (SOURCES
	advanced_userlist.cpp
	main.cpp
)

set (MOC_SOURCES
	advanced_userlist.h
)

set (TRANSLATION_SOURCES
	translations/advanced_userlist_en.ts
	translations/advanced_userlist_de.ts
	translations/advanced_userlist_fr.ts
	translations/advanced_userlist_it.ts
	translations/advanced_userlist_pl.ts
)

set (CONFIGURATION_FILES
	configuration/advanced_userlist.ui
)

kadu_module (advanced_userlist
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
