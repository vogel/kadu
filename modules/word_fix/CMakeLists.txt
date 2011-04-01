set (SOURCES
	word_fix.cpp
)

set (MOC_SOURCES
	word_fix.h
)

set (TRANSLATION_SOURCES
	translations/word_fix_en.ts
	translations/word_fix_pl.ts
)

set (CONFIGURATION_FILES
	configuration/word_fix.ui
)

set (DATA_FILES
	data/wf_default_list.data
)

kadu_module (word_fix
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/word_fix
)
