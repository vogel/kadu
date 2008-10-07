set (SOURCES
	wizard.cpp
)

set (MOC_SOURCES
	wizard.h
)

set (TRANSLATION_SOURCES
	translations/config_wizard_de.ts
	translations/config_wizard_fr.ts
	translations/config_wizard_it.ts
	translations/config_wizard_pl.ts
)

set (DATA_DIRECTORIES
	data/joi
	data/ronk2
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (config_wizard.desc)
qt_wrap_cpp (config_wizard MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

install (DIRECTORY ${DATA_FILES}
	DESTINATION share/kadu/modules/data/config_wizard
	PATTERN ".svn" EXCLUDE
)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (config_wizard ${config_wizard} ${SOURCES} ${MOC_FILES})
add_custom_target (config_wizard-translations DEPENDS ${TRANSLATION_FILES})
if (config_wizard STREQUAL STATIC)
	set_target_properties(config_wizard PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (config_wizard STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(config_wizard kadu_core sound)
	endif (WIN32)
	if (APPLE)
		set_target_properties(config_wizard PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (config_wizard STREQUAL STATIC)
add_dependencies (config_wizard config_wizard-translations)

install (TARGETS config_wizard DESTINATION lib/kadu/modules)
