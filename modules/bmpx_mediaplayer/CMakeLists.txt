set (SOURCES
	bmpx.cpp
)

set (MOC_SOURCES
	bmpx.h
)

kadu_module (bmpx_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
