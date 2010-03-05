set (SOURCES
	cenzor.cpp)

set (MOC_SOURCES
	cenzor.h)

set (TRANSLATION_SOURCES
	translations/cenzor_pl.ts)

set (CONFIGURATION_FILES
	configuration/cenzor.ui)

set (DATA_FILES
	data/cenzor_words.conf
	data/cenzor_words_ok.conf)

kadu_module (cenzor
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/cenzor)

