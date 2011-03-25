set (SOURCES
	pcspeaker.cpp
	pcspeaker_configuration_widget.cpp
)

set (MOC_SOURCES
	pcspeaker.h
	pcspeaker_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/pcspeaker_pl.ts
	translations/pcspeaker_de.ts
	translations/pcspeaker_it.ts
)

#set (CONFIGURATION_FILES
#	configuration/pcspeaker.ui
#)

kadu_plugin (pcspeaker
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
