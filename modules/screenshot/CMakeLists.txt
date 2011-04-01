set (SOURCES
	configuration/gui/screenshot-configuration-ui-handler.cpp
	configuration/screen-shot-configuration.cpp
	gui/graphics-items/handler-rect-item.cpp
	gui/graphics-items/selection-frame-item.cpp
	gui/widgets/crop-image-widget.cpp
	gui/widgets/screenshot-tool-box.cpp
	gui/widgets/screenshot-widget.cpp
	notify/screenshot-notification.cpp
	pixmap-grabber.cpp
	screenshot.cpp
	screenshot-actions.cpp
	screenshot-module.cpp
	screen-shot-saver.cpp
	screenshot-taker.cpp
)

set (MOC_SOURCES
	configuration/gui/screenshot-configuration-ui-handler.h
	gui/graphics-items/handler-rect-item.h
	gui/widgets/crop-image-widget.h
	gui/widgets/screenshot-tool-box.h
	gui/widgets/screenshot-widget.h
	notify/screenshot-notification.h
	screenshot.h
	screenshot-actions.h
	screen-shot-saver.h
	screenshot-taker.h
)

set (TRANSLATION_SOURCES
	translations/screenshot_en.ts
	translations/screenshot_de.ts
	translations/screenshot_fr.ts
	translations/screenshot_it.ts
	translations/screenshot_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/screenshot.ui
)

kadu_module (screenshot
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
