set (SOURCES
	gui/windows/file-transfer-window.cpp
	gui/widgets/file-transfer-widget.cpp

	file-transfer-module.cpp
)

set (MOC_SOURCES
	gui/windows/file-transfer-window.h
	gui/widgets/file-transfer-widget.h

	file-transfer-module.h
)

set (TRANSLATION_SOURCES
	translations/dcc_de.ts
	translations/dcc_fr.ts
	translations/dcc_it.ts
	translations/dcc_pl.ts
)

set (CONFIGURATION_FILES
	configuration/dcc.ui
)

kadu_module (file_transfer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF DCC_BUILD
)
