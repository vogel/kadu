set (SOURCES
	last-seen-plugin.cpp
	infos.cpp
	infos_dialog.cpp
)

set (MOC_SOURCES
	last-seen-plugin.h
	infos.h
	infos_dialog.h
)

set (TRANSLATION_SOURCES
	translations/last_seen_en.ts
	translations/last_seen_de.ts
	translations/last_seen_fr.ts
	translations/last_seen_it.ts
	translations/last_seen_pl.ts
)

kadu_plugin (last_seen
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
