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

include (FindPkgConfig)
pkg_search_module (ENCHANT enchant)
if (NOT ENCHANT_FOUND OR SPELLCHECKER_USE_ASPELL)
	include (FindASPELL)
	if (NOT ASPELL_FOUND)
		message (FATAL_ERROR "enchant or aspell not found!!")
	endif (NOT ASPELL_FOUND)
endif (NOT ENCHANT_FOUND OR SPELLCHECKER_USE_ASPELL)

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

if (ENCHANT_FOUND AND NOT SPELLCHECKER_USE_ASPELL)
	message (STATUS "Using enchant as backend. To force aspell use -DSPELLCHECKER_USE_ASPELL=yes")
	target_link_libraries (spellchecker ${ENCHANT_LIBRARIES})
	include_directories (${ENCHANT_INCLUDE_DIRS})
	add_definitions (${ENCHANT_DEFINITIONS})
	add_definitions (-DHAVE_ENCHANT)
endif (ENCHANT_FOUND AND NOT SPELLCHECKER_USE_ASPELL)

if (ASPELL_FOUND)
	message (STATUS "Using aspell as backend")
	target_link_libraries (spellchecker ${ASPELL_LIBRARIES})
	include_directories (${ASPELL_INCLUDE_DIRS})
	add_definitions (${ASPELL_DEFINITIONS})
	add_definitions (-DHAVE_ASPELL)
endif (ASPELL_FOUND)

if (spellchecker STREQUAL STATIC)
	set_target_properties(spellchecker PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (spellchecker STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(spellchecker kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(spellchecker PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS spellchecker DESTINATION ${MODULEDIR})
endif (spellchecker STREQUAL STATIC)

