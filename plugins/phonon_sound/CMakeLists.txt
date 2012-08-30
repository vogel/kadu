project (phonon_sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	phonon-player.cpp
	phonon-plugin.cpp
)

set (MOC_SOURCES
	phonon-player.h
	phonon-plugin.h
)

pkg_search_module (PHONON phonon)
if (PHONON_FOUND)
	set (LIBRARIES ${PHONON_LIBRARIES})
	link_directories (${PHONON_LIBRARY_DIRS})
	include_directories (${PHONON_INCLUDE_DIRS})
else (PHONON_FOUND)
	if (QT_PHONON_FOUND)
		set (LIBRARIES ${QT_PHONON_LIBRARY})
		include_directories (${QT_PHONON_INCLUDE_DIR})
	else (QT_PHONON_FOUND)
		message (FATAL_ERROR "Could not find Phonon library")
	endif (QT_PHONON_FOUND)
endif (PHONON_FOUND)

message (STATUS "Found Phonon library: ${LIBRARIES}")

kadu_plugin (phonon_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_DEPENDENCIES sound
)
