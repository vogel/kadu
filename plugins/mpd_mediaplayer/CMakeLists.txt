project (mpd_mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	mpd-config.cpp
	mpd-mediaplayer.cpp
	mpd-player-module.cpp
	mpd-player-plugin-modules-factory.cpp
	mpd-player-plugin-object.cpp
)

set (CONFIGURATION_FILES
	configuration/mpd_config.ui
)

find_package (PkgConfig)
pkg_search_module (LIBMPDCLIENT REQUIRED libmpdclient)
include_directories (${LIBMPDCLIENT_INCLUDE_DIRS})
link_directories (${LIBMPDCLIENT_LIBRARY_DIRS})

kadu_plugin (mpd_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${LIBMPDCLIENT_LIBRARIES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES mediaplayer
)
