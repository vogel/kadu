set (SOURCES
	pcspeaker.cpp
	pcspeaker_configuration_widget.cpp
)

set (MOC_SOURCES
	pcspeaker.h
	pcspeaker_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/pcspeaker_en.ts
	translations/pcspeaker_pl.ts
	translations/pcspeaker_de.ts
	translations/pcspeaker_it.ts
)

#set (CONFIGURATION_FILES
#	configuration/pcspeaker.ui
#)

kadu_module (pcspeaker
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
