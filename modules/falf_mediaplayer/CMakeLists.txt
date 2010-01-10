set (SOURCES
	falf.cpp
)

set (MOC_SOURCES
	falf.h
)

kadu_module (falf_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
)
