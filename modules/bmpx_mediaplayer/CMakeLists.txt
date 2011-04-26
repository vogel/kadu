set (SOURCES
	bmpx.cpp
)

set (MOC_SOURCES
	bmpx.h
)

kadu_plugin (bmpx_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
