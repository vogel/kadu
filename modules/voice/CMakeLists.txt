set (SOURCES
	voice.cpp
)

set (MOC_SOURCES
	voice.h
)

set (TRANSLATION_SOURCES
	translations/voice_de.ts
	translations/voice_fr.ts
	translations/voice_it.ts
	translations/voice_pl.ts
)

set (CONFIGURATION_FILES
	configuration/voice.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (voice.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (voice MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_subdirectory (libgsm)

add_library (voice ${voice} ${SOURCES} ${MOC_FILES})
add_custom_target (voice-translations DEPENDS ${TRANSLATION_FILES})
if (voice STREQUAL STATIC)
	set_target_properties(voice PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (voice STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(voice kadu_core gsm dcc sound)
	else (WIN32)
target_link_libraries(voice gsm)
endif (WIN32)
endif (voice STREQUAL STATIC)
add_dependencies (voice voice-translations)

install (TARGETS voice DESTINATION lib/kadu/modules)
