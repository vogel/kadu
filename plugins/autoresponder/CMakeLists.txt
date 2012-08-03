project (autoresponder)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	autoresponder.cpp
	autoresponder-configuration.cpp
	autoresponder-configuration-ui-handler.cpp
	autoresponder-configurator.cpp
)

set (MOC_SOURCES
	autoresponder.h
	autoresponder-configuration-ui-handler.h
)

set (TRANSLATION_SOURCES
	translations/autoresponder_cs.ts
	translations/autoresponder_de.ts
	translations/autoresponder_en.ts
	translations/autoresponder_it.ts
	translations/autoresponder_pl.ts
)

set (CONFIGURATION_FILES
	configuration/autoresponder.ui
)

kadu_plugin (autoresponder
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
