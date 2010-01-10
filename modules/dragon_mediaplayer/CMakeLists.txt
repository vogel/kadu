set (SOURCES
	dragon.cpp
)

set (MOC_SOURCES
	dragon.h
)

kadu_module (dragon_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_LIBRARIES ${QT_QTDBUS_LIBRARY}
)
