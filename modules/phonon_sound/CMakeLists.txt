set (SOURCES
	phonon_sound.cpp
)

set (MOC_SOURCES
	phonon_sound.h
)

kadu_module (qt4_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
)
