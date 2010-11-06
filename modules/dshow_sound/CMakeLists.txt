set (SOURCES
	player_dshow.cpp
)

set (MOC_SOURCES
	player_dshow.h
)

kadu_module (dshow_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_DEPENDENCIES sound
)
