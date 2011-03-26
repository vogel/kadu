set (SOURCES
	speech-configuration-ui-handler.cpp
	speech-configuration-widget.cpp
	speech-module.cpp
	speech.cpp
)

set (MOC_SOURCES
	speech-configuration-ui-handler.h
	speech-configuration-widget.h
	speech.h
)

set (TRANSLATION_SOURCES
	translations/speech_de.ts
	translations/speech_fr.ts
	translations/speech_it.ts
	translations/speech_pl.ts
)

set (CONFIGURATION_FILES
	configuration/speech.ui
)

kadu_plugin (speech
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
