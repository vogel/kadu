set (SOURCES
	infos.cpp
	infos_dialog.cpp
)

set (MOC_SOURCES
	infos.h
	infos_dialog.h
)

set (TRANSLATION_SOURCES
	translations/last_seen_de.ts
	translations/last_seen_fr.ts
	translations/last_seen_it.ts
	translations/last_seen_pl.ts
)

kadu_module (last_seen
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
