set (SOURCES
	default_sms.cpp
)

set (MOC_SOURCES
	default_sms.h
)

set (TRANSLATION_SOURCES
	translations/default_sms_de.ts
	translations/default_sms_fr.ts
	translations/default_sms_it.ts
	translations/default_sms_pl.ts
)

set (CONFIGURATION_FILES
	configuration/default_sms.ui)

kadu_module (default_sms
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
