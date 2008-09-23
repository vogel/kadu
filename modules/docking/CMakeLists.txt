set (SOURCES
	docking.cpp
)

set (MOC_SOURCES
	docking.h
)

set (TRANSLATION_SOURCES
	translations/docking_de.ts
	translations/docking_fr.ts
	translations/docking_it.ts
	translations/docking_pl.ts
)

set (CONFIGURATION_FILES
	configuration/docking.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (docking.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (docking MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (docking ${docking} ${SOURCES} ${MOC_FILES})
set_target_properties(docking PROPERTIES COMPILE_FLAGS "-DDOCKING_BUILD" )
add_custom_target (docking-translations DEPENDS ${TRANSLATION_FILES})
if (docking STREQUAL STATIC)
	set_target_properties(docking PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (docking STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(docking kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(docking PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (docking STREQUAL STATIC)

add_dependencies (docking docking-translations)

install (TARGETS docking DESTINATION lib/kadu/modules)
