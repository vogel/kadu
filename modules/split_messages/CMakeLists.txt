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

kadu_plugin (split_messages
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
