set (SOURCES
	player_dshow.cpp
)

set (MOC_SOURCES
	player_dshow.h
)

set (TRANSLATION_SOURCES
	translations/player_dshow_de.ts
	translations/player_dshow_fr.ts
	translations/player_dshow_it.ts
	translations/player_dshow_pl.ts
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (dshow_sound.desc)
qt_wrap_cpp (dshow_sound MOC_FILES ${MOC_SOURCES})

#qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

#install (FILES ${TRANSLATION_FILES}
#	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (dshow_sound ${dshow_sound} ${SOURCES} ${MOC_FILES})
#add_custom_target (dshow_sound-translations DEPENDS ${TRANSLATION_FILES})
if (dshow_sound STREQUAL STATIC)
set_target_properties(dshow_sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (dshow_sound STREQUAL STATIC)
target_link_libraries(dshow_sound kadu_core sound)
endif (dshow_sound STREQUAL STATIC)
#add_dependencies (dshow_sound dshow_sound-translations)

install (TARGETS dshow_sound DESTINATION lib/kadu/modules)
