project (mpd_mediaplayer)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	mpd-mediaplayer.cpp
	mpd-config.cpp
)

set (MOC_SOURCES
	mpd-mediaplayer.h
)

set (CONFIGURATION_FILES
	configuration/mpd_config.ui
)

pkg_search_module (LIBMPDCLIENT REQUIRED libmpdclient)
include_directories (${LIBMPDCLIENT_INCLUDE_DIRS})
link_directories (${LIBMPDCLIENT_LIBRARY_DIRS})

kadu_plugin (mpd_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_LIBRARIES ${LIBMPDCLIENT_LIBRARIES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
