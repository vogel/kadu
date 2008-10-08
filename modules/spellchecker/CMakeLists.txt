set (SOURCES
	spellchecker.cpp
	highlighter.cpp
)

set (MOC_SOURCES
	spellchecker.h
)

set (TRANSLATION_SOURCES
	translations/spellchecker_pl.ts
)

set (CONFIGURATION_FILES
	configuration/spellchecker.ui)

include (FindASPELL)

if (NOT ASPELL_FOUND)
	message (FATAL_ERROR "libaspell not found!!")
endif (NOT ASPELL_FOUND)

kadu_module_desc (spellchecker.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (spellchecker MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (spellchecker ${spellchecker} ${SOURCES} ${MOC_FILES})
add_custom_target (spellchecker-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (spellchecker spellchecker-translations)

target_link_libraries (spellchecker ${ASPELL_LIBRARIES})
include_directories (${ASPELL_INCLUDE_DIRS})
add_definitions (${ASPELL_DEFINITIONS})

install (TARGETS spellchecker DESTINATION lib/kadu/modules)
