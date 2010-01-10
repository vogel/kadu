set (SOURCES
	mpris.cpp
)

set (MOC_SOURCES
	mpris.h
)

set (TRANSLATION_SOURCES
	translations/mpris_mediaplayer_pl.ts)

set (CONFIGURATION_FILES
	configuration/mpris_mediaplayer.ui)

kadu_module_desc (mpris_mediaplayer.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (mpris_mediaplayer MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (mpris_mediaplayer ${mpris_mediaplayer} ${SOURCES} ${MOC_FILES})
add_custom_target (mpris_mediaplayer-translations DEPENDS ${TRANSLATION_FILES})

target_link_libraries (mpris_mediaplayer ${QT_QTDBUS_LIBRARY})

if (mpris_mediaplayer STREQUAL STATIC)
	set_target_properties(mpris_mediaplayer PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (mpris_mediaplayer STREQUAL STATIC)
	install (TARGETS mpris_mediaplayer DESTINATION ${MODULEDIR})
endif (mpris_mediaplayer STREQUAL STATIC)

add_dependencies (mpris_mediaplayer mpris_mediaplayer-translations)

install (TARGETS mpris_mediaplayer DESTINATION ${MODULEDIR})
