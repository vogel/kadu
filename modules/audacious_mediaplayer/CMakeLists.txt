set (SOURCES
	audacious.cpp
)

set (MOC_SOURCES
	audacious.h
)

kadu_plugin (audacious_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${QT_QTDBUS_LIBRARY}
)

