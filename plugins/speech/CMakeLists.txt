project (speech)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	speech-configuration-ui-handler.cpp
	speech-configuration-widget.cpp
	speech-plugin.cpp
	speech.cpp
)

set (MOC_SOURCES
	speech-configuration-ui-handler.h
	speech-configuration-widget.h
	speech-plugin.h
	speech.h
)

set (CONFIGURATION_FILES
	configuration/speech.ui
)

kadu_plugin (speech
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
