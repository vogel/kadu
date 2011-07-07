project (cenzor)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/gui/cenzor-configuration-ui-handler.cpp
	configuration/cenzor-configuration.cpp
	gui/widgets/list-edit-widget.cpp
	notify/cenzor-notification.cpp

	cenzor.cpp
	cenzor-plugin.cpp
)

set (MOC_SOURCES
	configuration/gui/cenzor-configuration-ui-handler.h
	gui/widgets/list-edit-widget.h
	notify/cenzor-notification.h

	cenzor.h
	cenzor-plugin.h
)

set (TRANSLATION_SOURCES
	translations/cenzor_cs.ts
	translations/cenzor_de.ts
	translations/cenzor_en.ts
	translations/cenzor_pl.ts
	translations/cenzor_tr.ts
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
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
