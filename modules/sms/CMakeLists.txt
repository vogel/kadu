set (SOURCES
	sms.cpp
)

set (MOC_SOURCES
	sms.h
)

set (TRANSLATION_SOURCES
	translations/sms_de.ts
	translations/sms_fr.ts
	translations/sms_it.ts
	translations/sms_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sms.ui)

kadu_module (sms
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
