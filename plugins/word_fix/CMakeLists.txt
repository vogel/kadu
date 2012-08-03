project (word_fix)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	word-fix.cpp
)

set (MOC_SOURCES
	word-fix.h
)

set (TRANSLATION_SOURCES
	translations/word_fix_cs.ts
	translations/word_fix_de.ts
	translations/word_fix_en.ts
	translations/word_fix_es_ES.ts
	translations/word_fix_pl.ts
	translations/word_fix_ru.ts
)

set (CONFIGURATION_FILES
	configuration/word_fix.ui
)

set (DATA_FILES
	data/wf_default_list.data
)

kadu_plugin (word_fix
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
