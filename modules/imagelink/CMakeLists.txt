set (SOURCES
	imagelink.cpp
)

set (MOC_SOURCES
	imagelink.h
)

set (CONFIGURATION_FILES
	configuration/imagelink.ui
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
