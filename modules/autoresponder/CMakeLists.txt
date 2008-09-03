set (SOURCES
	autoresponder.cpp
)

set (MOC_SOURCES
	autoresponder.h
)

set (TRANSLATION_SOURCES
	translations/autoresponder_de.ts
	translations/autoresponder_fr.ts
	translations/autoresponder_it.ts
	translations/autoresponder_pl.ts
)

set (CONFIGURATION_FILES
	configuration/autoresponder.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (autoresponder.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (autoresponder MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (autoresponder ${autoresponder} ${SOURCES} ${MOC_FILES})
add_custom_target (autoresponder-translations DEPENDS ${TRANSLATION_FILES})
if (autoresponder STREQUAL STATIC)
	set_target_properties(autoresponder PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (autoresponder STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(autoresponder kadu_core)
	endif (WIN32)
endif (autoresponder STREQUAL STATIC)
add_dependencies (autoresponder autoresponder-translations)

install (TARGETS autoresponder DESTINATION lib/kadu/modules)
