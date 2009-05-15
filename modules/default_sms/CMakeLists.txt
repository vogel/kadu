set (SOURCES
	default_sms.cpp
)

set (MOC_SOURCES
	default_sms.h
)

set (TRANSLATION_SOURCES
	translations/default_sms_de.ts
	translations/default_sms_fr.ts
	translations/default_sms_it.ts
	translations/default_sms_pl.ts
)

set (CONFIGURATION_FILES
	configuration/default_sms.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (default_sms.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (default_sms MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (default_sms ${default_sms} ${SOURCES} ${MOC_FILES})
add_custom_target (default_sms-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (default_sms default_sms-translations)
if (default_sms STREQUAL STATIC)
	set_target_properties(default_sms PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (default_sms STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(default_sms kadu_core sms)
	endif (WIN32)
	if (APPLE)
		set_target_properties(default_sms PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS default_sms DESTINATION ${MODULEDIR})
endif (default_sms STREQUAL STATIC)

