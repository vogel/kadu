set (SOURCES
	autostatus.cpp
	power_status_changer.cpp
)

set (MOC_SOURCES
	autostatus.h
	power_status_changer.h
)

set (TRANSLATION_SOURCES
	translations/autostatus_pl.ts)

set (CONFIGURATION_FILES
	configuration/autostatus.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (autostatus.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (autostatus MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (autostatus ${autostatus} ${SOURCES} ${MOC_FILES})
add_custom_target (autostatus-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (autostatus autostatus-translations)
if (autostatus STREQUAL STATIC)
	set_target_properties(autostatus PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (autostatus STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(autostatus kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(autostatus PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS autostatus DESTINATION ${MODULEDIR})
endif (autostatus STREQUAL STATIC)

