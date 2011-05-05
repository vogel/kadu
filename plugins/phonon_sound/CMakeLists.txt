project (phonon_sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

include_directories (${KADU_INCLUDE_DIR})
include_directories (${KADU_INCLUDE_DIR}/kadu-core)

set (SOURCES
	phonon-player.cpp
	phonon-plugin.cpp
)

set (MOC_SOURCES
	phonon-player.h
	phonon-plugin.h
)

set (QT_USE_PHONON 1)

if (NOT QT_PHONON_FOUND)
	pkg_search_module (PHONON REQUIRED phonon)
endif (NOT QT_PHONON_FOUND)

kadu_plugin (phonon_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QT_PHONON_LIBRARY}
	PLUGIN_DEPENDENCIES sound
)
