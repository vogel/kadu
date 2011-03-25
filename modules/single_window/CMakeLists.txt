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

kadu_plugin (single_window
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
