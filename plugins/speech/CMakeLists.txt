project (speech)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	speech-configuration-ui-handler.cpp
	speech-configuration-widget.cpp
	speech-module.cpp
	speech-plugin-modules-factory.cpp
	speech-plugin-object.cpp
	speech.cpp
)

set (CONFIGURATION_FILES
	configuration/speech.ui
)

kadu_plugin (speech
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
