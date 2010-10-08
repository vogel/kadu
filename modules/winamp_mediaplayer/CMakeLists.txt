set (SOURCES
	winamp.cpp
)

set (MOC_SOURCES
	winamp.h
)

kadu_module (winamp_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_DEPENDENCIES mediaplayer
)
