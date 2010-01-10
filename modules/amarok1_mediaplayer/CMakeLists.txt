set (SOURCES
	amarok.cpp
)

set (MOC_SOURCES
	amarok.h
)

kadu_module (amarok1_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
)
