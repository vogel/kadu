set (SOURCES
	mediaplayer.cpp
	mp_status_changer.cpp
)

set (MOC_SOURCES
	mediaplayer.h
	mp_status_changer.h
	player_commands.h
	player_info.h
)
	
if (UNIX AND NOT APPLE)

set (SOURCES ${SOURCES}
	mpris_controller.cpp
	mpris_mediaplayer.cpp
)

set (MOC_SOURCES ${MOC_SOURCES}
	mpris_controller.h
	mpris_mediaplayer.h
)

endif (UNIX AND NOT APPLE)


set (TRANSLATION_SOURCES
	translations/mediaplayer_pl.ts
)

set (CONFIGURATION_FILES
	configuration/mediaplayer.ui)

set (DATA_FILES
	data/mediaplayer.png
)

if (WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif (WIN32)

install (FILES ${DATA_FILES}
	DESTINATION "share/kadu/modules/data/mediaplayer")


kadu_module (mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${QT_QTDBUS_LIBRARY}
	MODULE_BUILDDEF MEDIAPLAYER_BUILD
)
