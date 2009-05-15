set (SOURCES
	antistring.cpp)

set (MOC_SOURCES
	antistring.h)

set (TRANSLATION_SOURCES
	translations/antistring_pl.ts)

set (CONFIGURATION_FILES
	configuration/antistring.ui)

set (DATA_FILES
	data/ant_conditions.conf)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (antistring.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (antistring MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/antistring)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (antistring ${antistring} ${SOURCES} ${MOC_FILES})
add_custom_target (antistring-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (antistring antistring-translations)
if (antistring STREQUAL STATIC)
	set_target_properties(antistring PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (antistring STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(antistring kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(antistring PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS antistring DESTINATION ${MODULEDIR})
endif (antistring STREQUAL STATIC)

