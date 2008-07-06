set (SOURCES
	autoaway.cpp
)

set (MOC_SOURCES
	autoaway.h
)

set (TRANSLATION_SOURCES
	translations/autoaway_de.ts
	translations/autoaway_fr.ts
	translations/autoaway_it.ts
	translations/autoaway_pl.ts
)

set (CONFIGURATION_FILES
	configuration/autoaway.ui)

kadu_module_desc (autoaway.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (autoaway MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (autoaway ${autoaway} ${SOURCES} ${MOC_FILES})
add_custom_target (autoaway-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (autoaway autoaway-translations)

install (TARGETS autoaway DESTINATION lib/kadu/modules)
