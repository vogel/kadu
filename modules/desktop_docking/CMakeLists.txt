set (SOURCES
	desktopdock.cpp
)

set (MOC_SOURCES
	desktopdock.h
)

set (TRANSLATION_SOURCES
	translations/desktop_docking_de.ts
	translations/desktop_docking_fr.ts
	translations/desktop_docking_it.ts
	translations/desktop_docking_pl.ts
)

set (CONFIGURATION_FILES
	configuration/desktop_docking.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (desktop_docking.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (desktop_docking MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (desktop_docking ${desktop_docking} ${SOURCES} ${MOC_FILES})
add_custom_target (desktop_docking-translations DEPENDS ${TRANSLATION_FILES})
if (desktop_docking STREQUAL STATIC)
	set_target_properties(desktop_docking PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (desktop_docking STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(desktop_docking kadu_core docking)
	endif (WIN32)
endif (desktop_docking STREQUAL STATIC)
add_dependencies (desktop_docking desktop_docking-translations)

install (TARGETS desktop_docking DESTINATION lib/kadu/modules)
