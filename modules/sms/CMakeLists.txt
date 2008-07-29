set (SOURCES
	sms.cpp
)

set (MOC_SOURCES
	sms.h
)

set (TRANSLATION_SOURCES
	translations/sms_de.ts
	translations/sms_fr.ts
	translations/sms_it.ts
	translations/sms_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sms.ui)

kadu_module_desc (sms.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (sms MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (sms ${sms} ${SOURCES} ${MOC_FILES})
set_target_properties(sms PROPERTIES COMPILE_FLAGS "-DSMS_BUILD" )
add_custom_target (sms-translations DEPENDS ${TRANSLATION_FILES})
if (sms STREQUAL STATIC)
set_target_properties(sms PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (sms STREQUAL STATIC)
if (WIN32)
target_link_libraries(sms kadu_core history)
endif (WIN32)
endif (sms STREQUAL STATIC)
add_dependencies (sms sms-translations)

install (TARGETS sms DESTINATION lib/kadu/modules)
