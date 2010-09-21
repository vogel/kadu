set (SOURCES
	kadu_speech.cpp
)

set (MOC_SOURCES
	kadu_speech.h
)

set (TRANSLATION_SOURCES
	translations/speech_de.ts
	translations/speech_fr.ts
	translations/speech_it.ts
	translations/speech_pl.ts
)

set (CONFIGURATION_FILES
	configuration/speech.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (speech.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (speech MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (speech ${speech} ${SOURCES} ${MOC_FILES})
add_custom_target (speech-translations DEPENDS ${TRANSLATION_FILES})
if (speech STREQUAL STATIC)
	set_target_properties(speech PROPERTIES COMPILE_FLAGS "-DKADULIB")
else (speech STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(speech kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(speech PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (speech STREQUAL STATIC)
add_dependencies (speech speech-translations)

install (TARGETS speech DESTINATION ${MODULEDIR})
