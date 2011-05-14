set (SOURCES
	mpd_mediaplayer.cpp
	mpd_config.cpp
)

set (MOC_SOURCES
	mpd_mediaplayer.h
)

set (CONFIGURATION_FILES
	configuration/mpd_config.ui
)

pkg_search_module (LIBMPDCLIENT REQUIRED libmpdclient)
include_directories (${LIBMPDCLIENT_INCLUDE_DIRS})

kadu_plugin (mpd_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${LIBMPDCLIENT_LIBRARIES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
