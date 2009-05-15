set (SOURCES
	screenshot.cpp
)

set (MOC_SOURCES
	screenshot.h
)

set (TRANSLATION_SOURCES
	translations/screenshot_pl.ts
)

set (CONFIGURATION_FILES
	configuration/screenshot.ui
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (screenshot.desc)
kadu_module_configuration (${CONFIGURATION_FILES})
qt_wrap_cpp (screenshot MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (screenshot ${screenshot} ${SOURCES} ${MOC_FILES})
add_custom_target (screenshot-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (screenshot screenshot-translations)
if (screenshot STREQUAL STATIC)
	set_target_properties(screenshot PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (screenshot STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(screenshot kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(screenshot PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS screenshot DESTINATION ${MODULEDIR})
endif (screenshot STREQUAL STATIC)

