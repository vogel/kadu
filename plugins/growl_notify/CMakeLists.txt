project (growl_notify)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	growl-notify.cpp
	growl-notify-configuration-widget.cpp
	growlnotifier.cpp
)

set (MOC_SOURCES
	growl-notify.h
	growl-notify-configuration-widget.h
	growlnotifier.h
)

set (TRANSLATION_SOURCES
	translations/growl_notify_cs.ts
	translations/growl_notify_de.ts
	translations/growl_notify_en.ts
	translations/growl_notify_pl.ts
)

set (GROWL_LIBRARIES "-framework Growl -framework CoreFoundation")

kadu_plugin (growl_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_LIBRARIES ${GROWL_LIBRARIES}
)
