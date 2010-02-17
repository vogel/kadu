set (SOURCES
	gui/widgets/crop-image-widget.cpp
	gui/widgets/screenshot-tool-box.cpp
	screenshot.cpp
)

set (MOC_SOURCES
	gui/widgets/crop-image-widget.h
	gui/widgets/screenshot-tool-box.h
	screenshot.h
)

set (TRANSLATION_SOURCES
	translations/screenshot_pl.ts
)

set (CONFIGURATION_FILES
	configuration/screenshot.ui
)

kadu_module (screenshot
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
