set (SOURCES
	mpd_mediaplayer.cpp
	mpd_config.cpp
)

set (MOC_SOURCES
	mpd_mediaplayer.h
	mpd_config.h
)

set (CONFIGURATION_FILES
	configuration/mpd_config.ui
)

pkg_search_module (LIBMPDCLIENT REQUIRED libmpdclient)
include_directories (${LIBMPDCLIENT_INCLUDE_DIRS})

kadu_module (mpd_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_LIBRARIES ${LIBMPDCLIENT_LIBRARIES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
