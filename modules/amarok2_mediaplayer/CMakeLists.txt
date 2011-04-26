set (SOURCES
	amarok2-mediaplayer-plugin.cpp
)

set (MOC_SOURCES
	amarok2-mediaplayer-plugin.h
)

kadu_plugin (amarok2_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
