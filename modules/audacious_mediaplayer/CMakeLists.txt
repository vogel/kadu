set (SOURCES
	audacious.cpp
)

set (MOC_SOURCES
	audacious.h
)

kadu_module (audacious_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_LIBRARIES ${QT_QTDBUS_LIBRARY}
)

