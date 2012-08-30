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

set (GROWL_LIBRARIES "-framework Growl -framework CoreFoundation")

kadu_plugin (growl_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${GROWL_LIBRARIES}
)
