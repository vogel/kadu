project (screenshot)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

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
	screenshot-plugin.cpp
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
	screenshot-plugin.h
	screen-shot-saver.h
	screenshot-taker.h
)

set (TRANSLATION_SOURCES
	translations/screenshot_cs.ts
	translations/screenshot_en.ts
	translations/screenshot_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/screenshot.ui
)

include (FindX11)
include_directories (${X11_INCLUDE_DIR})

kadu_plugin (screenshot
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
