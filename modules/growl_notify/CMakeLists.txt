set (SOURCES
	growl_notify.cpp
	growl_notify_configuration_widget.cpp
	growlnotifier.cpp
)

set (MOC_SOURCES
	growl_notify.h
	growl_notify_configuration_widget.h
	growlnotifier.h
)

set (TRANSLATION_SOURCES
	translations/growl_notify_en.ts
	translations/growl_notify_pl.ts
)

if (APPLE)
	set (LIBRARIES "-framework Growl -framework CoreFoundation")
endif (APPLE)

kadu_plugin (growl_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
