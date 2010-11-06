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

kadu_module (ao_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (AO REQUIRED ao)
include_directories (${AO_INCLUDE_DIRS})
link_directories (${AO_LIBRARY_DIRS})

target_link_libraries (ao_sound ${AO_LIBRARIES})
