set (SOURCES
	qt4_docking_notify.cpp
	qt4_docking_notify_configuration_widget.cpp
)

set (MOC_SOURCES
	qt4_docking_notify.h
	qt4_docking_notify_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/qt4_docking_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/qt4-docking-notify.ui
)

kadu_module (qt4_docking_notify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES qt4_docking
)
