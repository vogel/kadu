set (SOURCES
	qt4_docking_notify.cpp
	qt4_docking_notify_configuration_widget.cpp
	qt4-docking-notify-plugin.cpp
)

set (MOC_SOURCES
	qt4_docking_notify.h
	qt4_docking_notify_configuration_widget.h
	qt4-docking-notify-plugin.h
)

set (TRANSLATION_SOURCES
	translations/qt4_docking_notify_en.ts
	translations/qt4_docking_notify_de.ts
	translations/qt4_docking_notify_fr.ts
	translations/qt4_docking_notify_it.ts
	translations/qt4_docking_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/qt4-docking-notify.ui
)

kadu_plugin (qt4_docking_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES qt4_docking
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
