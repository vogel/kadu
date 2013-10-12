project (phonon_sound)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	phonon-player.cpp
	phonon-plugin.cpp
)

set (MOC_SOURCES
	phonon-player.h
	phonon-plugin.h
)

if (NOT WIN32)
	find_package (PkgConfig)
	pkg_search_module (PHONON phonon)
	if (PHONON_FOUND)
		set (LIBRARIES ${PHONON_LIBRARIES})
		link_directories (${PHONON_LIBRARY_DIRS})
		include_directories (${PHONON_INCLUDE_DIRS})
	endif ()
endif ()

if (WIN32 OR NOT PHONON_FOUND)
	if (QT_PHONON_FOUND)
		set (LIBRARIES ${QT_PHONON_LIBRARY})
		include_directories (${QT_PHONON_INCLUDE_DIR})
	else ()
		message (FATAL_ERROR "Could not find Phonon library")
	endif ()
endif ()

message (STATUS "Found Phonon library: ${LIBRARIES}")

kadu_plugin (phonon_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_DEPENDENCIES sound
)
