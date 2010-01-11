set (SOURCES
	itunes.cpp
	itunescontroller.cpp
)

set (MOC_SOURCES
	itunes.h
)

kadu_module (itunes_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
)
