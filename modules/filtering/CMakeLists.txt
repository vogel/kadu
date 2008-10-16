set (SOURCES
	filtering.cpp
)

set (MOC_SOURCES
	filtering.h
)

set (TRANSLATION_SOURCES
	translations/filtering_pl.ts
)

set (CONFIGURATION_FILES
	configuration/filtering.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (filtering.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (filtering MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (filtering ${filtering} ${SOURCES} ${MOC_FILES})
add_custom_target (filtering-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (filtering filtering-translations)
if (filtering STREQUAL STATIC)
	set_target_properties(filtering PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (filtering STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(autoresponder kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(ext_sound PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS filtering DESTINATION lib/kadu/modules)
endif (filtering STREQUAL STATIC)

install (FILES "data/clear.png" "data/filtering.png" DESTINATION "share/kadu/modules/data/filtering")
