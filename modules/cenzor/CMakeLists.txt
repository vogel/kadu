set (SOURCES
	configuration/gui/cenzor-configuration-ui-handler.cpp
	configuration/cenzor-configuration.cpp
	gui/widgets/list-edit-widget.cpp
	notify/cenzor-notification.cpp

	cenzor.cpp
	cenzor-module.cpp
)

set (MOC_SOURCES
	configuration/gui/cenzor-configuration-ui-handler.h
	gui/widgets/list-edit-widget.h
	notify/cenzor-notification.h

	cenzor.h
)

set (TRANSLATION_SOURCES
	translations/cenzor_en.ts
	translations/cenzor_de.ts
	translations/cenzor_fr.ts
	translations/cenzor_it.ts
	translations/cenzor_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/cenzor.ui
)

set (DATA_FILES
	data/cenzor_words.conf
	data/cenzor_words_ok.conf
)

kadu_module (cenzor
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/cenzor
)
