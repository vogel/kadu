set (SOURCES
	single_window.cpp
)

set (MOC_SOURCES
	single_window.h
)

set (TRANSLATION_SOURCES
	translations/single_window_de.ts
	translations/single_window_fr.ts
	translations/single_window_it.ts
	translations/single_window_pl.ts
)

set (CONFIGURATION_FILES
	configuration/single_window.ui
)

kadu_module (single_window
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES docking
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
