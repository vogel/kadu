set (SOURCES
	configuration/image-link-configuration.cpp

	image-link.cpp
	image-link-module.cpp
)

set (MOC_SOURCES
	image-link.h
)

set (CONFIGURATION_FILES
	data/configuration/image-link.ui
)

set (TRANSLATION_SOURCES
	translations/imagelink_de.ts
	translations/imagelink_fr.ts
	translations/imagelink_it.ts
	translations/imagelink_pl.ts
)

kadu_plugin (imagelink
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
