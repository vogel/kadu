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
	translations/imagelink_pl.ts
)

kadu_module (imagelink
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
