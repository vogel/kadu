set (SOURCES
	wizard.cpp
)

set (MOC_SOURCES
	wizard.h
)

set (TRANSLATION_SOURCES
	translations/config_wizard_de.ts
	translations/config_wizard_fr.ts
	translations/config_wizard_it.ts
	translations/config_wizard_pl.ts
)

kadu_module_desc (config_wizard.desc)
qt_wrap_cpp (config_wizard MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (config_wizard ${config_wizard} ${SOURCES} ${MOC_FILES})
add_custom_target (config_wizard-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (config_wizard config_wizard-translations)

install (TARGETS config_wizard DESTINATION lib/kadu/modules)
