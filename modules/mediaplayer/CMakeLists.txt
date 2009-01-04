set (SOURCES
	mediaplayer.cpp
	mp_status_changer.cpp
	mpris_controller.cpp
	mpris_mediaplayer.cpp
)

set (MOC_SOURCES
	mediaplayer.h
	mp_status_changer.h
	player_commands.h
	player_info.h
	mpris_controller.h
	mpris_mediaplayer.h
)

set (TRANSLATION_SOURCES
	translations/mediaplayer_pl.ts
)

set (CONFIGURATION_FILES
	configuration/mediaplayer.ui)

if (WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif (WIN32)

kadu_module_desc (mediaplayer.desc)
kadu_module_configuration (${CONFIGURATION_FILES})
qt_wrap_cpp (mediaplayer MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (mediaplayer ${mediaplayer} ${SOURCES} ${MOC_FILES})
add_custom_target (mediaplayer-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (mediaplayer mediaplayer-translations)
if (mediaplayer STREQUAL STATIC)
	set_target_properties(mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (mediaplayer STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(mediaplayer kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(mediaplayer PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS mediaplayer DESTINATION lib/kadu/modules)
endif (mediaplayer STREQUAL STATIC)

