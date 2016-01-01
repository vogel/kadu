project (winamp_mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	winamp.cpp
	winamp-player-plugin.cpp
)

if (MINGW)
	set_property (SOURCE winamp.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -fno-strict-aliasing")
endif ()

kadu_plugin (winamp_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_DEPENDENCIES mediaplayer
)
