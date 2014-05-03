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
	pkg_search_module (PHONON phonon4qt5)
	if (PHONON_FOUND)
		set (LIBRARIES ${PHONON_LIBRARIES})
		link_directories (${PHONON_LIBRARY_DIRS})
		include_directories (${PHONON_INCLUDE_DIRS})
	endif ()
endif ()

if (WIN32 OR NOT PHONON_FOUND)
	if (QT_PHONON_FOUND)
		set (PHONON_MODULE phonon4qt5)
	else ()
		message (FATAL_ERROR "Could not find Phonon library")
	endif ()
endif ()

kadu_plugin (phonon_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_DEPENDENCIES sound
	PLUGIN_ADDITIONAL_QT_MODULES ${PHONON_MODULE}
)
