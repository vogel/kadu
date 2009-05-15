set (SOURCES
	word_fix.cpp)

set (MOC_SOURCES
	word_fix.h)

set (TRANSLATION_SOURCES
	translations/word_fix_pl.ts)

set (CONFIGURATION_FILES
	configuration/word_fix.ui)

set (DATA_FILES
	data/wf_default_list.data)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (word_fix.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (word_fix MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/word_fix)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (word_fix ${word_fix} ${SOURCES} ${MOC_FILES})
add_custom_target (word_fix-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (word_fix word_fix-translations)
if (word_fix STREQUAL STATIC)
	set_target_properties(word_fix PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (word_fix STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(word_fix kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(word_fix PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS word_fix DESTINATION ${MODULEDIR})
endif (word_fix STREQUAL STATIC)

