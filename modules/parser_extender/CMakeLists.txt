set (SOURCES
	parser_extender.cpp)

set (MOC_SOURCES
	parser_extender.h)

set (TRANSLATION_SOURCES
	translations/parser_extender_pl.ts)

set (CONFIGURATION_FILES
	configuration/parser_extender.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (parser_extender.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (parser_extender MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (parser_extender ${parser_extender} ${SOURCES} ${MOC_FILES})
add_custom_target (parser_extender-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (parser_extender parser_extender-translations)
if (parser_extender STREQUAL STATIC)
	set_target_properties(parser_extender PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (parser_extender STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(parser_extender kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(parser_extender PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS parser_extender DESTINATION ${MODULEDIR})
endif (parser_extender STREQUAL STATIC)

