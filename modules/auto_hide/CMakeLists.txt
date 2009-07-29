set (SOURCES
	auto_hide.cpp)

set (MOC_SOURCES
	auto_hide.h)

set (TRANSLATION_SOURCES
	translations/auto_hide_pl.ts)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (auto_hide.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (auto_hide MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (auto_hide ${auto_hide} ${SOURCES} ${MOC_FILES})
add_custom_target (auto_hide-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (auto_hide auto_hide-translations)
if (auto_hide STREQUAL STATIC)
	set_target_properties(auto_hide PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (auto_hide STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(auto_hide kadu_core idle)
	endif (WIN32)
	if (APPLE)
		set_target_properties(auto_hide PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS auto_hide DESTINATION ${MODULEDIR})
endif (auto_hide STREQUAL STATIC)

