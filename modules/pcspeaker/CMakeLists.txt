set (SOURCES
	pcspeaker.cpp
	pcspeaker_configuration_widget.cpp
)

set (MOC_SOURCES
	pcspeaker.h
	pcspeaker_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/pcspeaker_pl.ts
	translations/pcspeaker_de.ts
	translations/pcspeaker_it.ts
)

set (CONFIGURATION_FILES
	configuration/pcspeaker.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (pcspeaker.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (pcspeaker MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (pcspeaker ${pcspeaker} ${SOURCES} ${MOC_FILES})
add_custom_target (pcspeaker-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (pcspeaker pcspeaker-translations)
if (pcspeaker STREQUAL STATIC)
	set_target_properties(pcspeaker PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (pcspeaker STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(pcspeaker kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(pcspeaker PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS pcspeaker DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (pcspeaker STREQUAL STATIC)

