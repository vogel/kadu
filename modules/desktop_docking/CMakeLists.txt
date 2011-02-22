set (SOURCES
	configuration/gui/desktop-dock-configuration-ui-handler.cpp
	gui/windows/desktop-dock-window.cpp
	desktop-docking-module.cpp
	desktop-dock.cpp
)

set (MOC_SOURCES
	configuration/gui/desktop-dock-configuration-ui-handler.h
	gui/windows/desktop-dock-window.h
	desktop-dock.h
)

set (TRANSLATION_SOURCES
	translations/desktop_docking_cs.ts
	translations/desktop_docking_de.ts
	translations/desktop_docking_fr.ts
	translations/desktop_docking_it.ts
	translations/desktop_docking_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/desktop_docking.ui
)

kadu_module (desktop_docking
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES docking
)
