set (SOURCES
	split.cpp
)

set (MOC_SOURCES
	split.h
)

set (TRANSLATION_SOURCES
	translations/split_messages_pl.ts
)

set (CONFIGURATION_FILES
	configuration/split_messages.ui
)

kadu_module (split_messages
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
