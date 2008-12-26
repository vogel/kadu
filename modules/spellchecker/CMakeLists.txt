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

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

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

if (spellchecker STREQUAL STATIC)
	set_target_properties(spellchecker PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (spellchecker STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(spellchecker kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(spellchecker PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS spellchecker DESTINATION lib/kadu/modules)
endif (spellchecker STREQUAL STATIC)

