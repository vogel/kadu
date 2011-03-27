set (QT_USE_QTDBUS 1)

set (SOURCES
	freedesktop-notify.cpp
)

set (MOC_SOURCES
	freedesktop-notify.h
)

set (TRANSLATION_SOURCES
	translations/freedesktop_notify_de.ts
	translations/freedesktop_notify_fr.ts
	translations/freedesktop_notify_it.ts
	translations/freedesktop_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/freedesktop_notify.ui
)

kadu_plugin (freedesktop_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBGADU_LIBRARIES}
)

target_link_libraries (freedesktop_notify ${QT_QTDBUS_LIBRARY})
