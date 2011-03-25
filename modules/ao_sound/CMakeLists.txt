set (SOURCES
	ao-module.cpp
	ao-player.cpp
)

set (MOC_SOURCES
	ao-player.h
)

if (WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif (WIN32)

kadu_plugin (ao_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (AO REQUIRED ao)
include_directories (${AO_INCLUDE_DIRS})
link_directories (${AO_LIBRARY_DIRS})

target_link_libraries (ao_sound ${AO_LIBRARIES})
