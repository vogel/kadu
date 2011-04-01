set (SOURCES
	mediaplayer.cpp
	mp_status_changer.cpp
)

set (MOC_SOURCES
	mediaplayer.h
	mp_status_changer.h
	player_commands.h
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

	set (DBUS_LIBS
		${QT_QTDBUS_LIBRARY}
	)
endif (UNIX AND NOT APPLE)


set (TRANSLATION_SOURCES
	translations/mediaplayer_en.ts
	translations/mediaplayer_de.ts
	translations/mediaplayer_fr.ts
	translations/mediaplayer_it.ts
	translations/mediaplayer_pl.ts
)

set (CONFIGURATION_FILES
	configuration/mediaplayer.ui
)

set (DATA_FILES
	data/mediaplayer.png
)

kadu_module (mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${DBUS_LIBS}
	MODULE_BUILDDEF MEDIAPLAYER_BUILD
)

install (FILES ${DATA_FILES}
	DESTINATION "share/kadu/modules/data/mediaplayer"
)

if (WIN32)
	set_target_properties (mediaplayer PROPERTIES COMPILE_FLAGS "-DMEDIAPLAYER_BUILD")
endif (WIN32)
