set (SOURCES
	dragon.cpp
)

set (MOC_SOURCES
	dragon.h
)

kadu_plugin (dragon_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
