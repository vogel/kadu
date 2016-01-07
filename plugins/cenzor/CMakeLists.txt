project (cenzor)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	configuration/gui/cenzor-configuration-ui-handler.cpp
	configuration/cenzor-configuration.cpp
	gui/widgets/list-edit-widget.cpp
	notification/cenzor-notification.cpp

	cenzor-message-filter.cpp
	cenzor-module.cpp
	cenzor-plugin-modules-factory.cpp
	cenzor-plugin-object.cpp
)

set (CONFIGURATION_FILES
	data/configuration/cenzor.ui
)

set (DATA_FILES
	data/cenzor_words.conf
	data/cenzor_words_ok.conf
)

kadu_plugin (cenzor
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
