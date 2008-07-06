set (SOURCES
	hint.cpp
	hint_manager.cpp
	hints_configuration_widget.cpp
	hints.cpp
)

set (MOC_SOURCES
	hint.h
	hint_manager.h
	hints_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/hints_de.ts
	translations/hints_fr.ts
	translations/hints_it.ts
	translations/hints_pl.ts
)

set (CONFIGURATION_FILES
	configuration/hints.ui)

kadu_module_desc (hints.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (hints MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (hints ${hints} ${SOURCES} ${MOC_FILES})
add_custom_target (hints-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (hints hints-translations)

install (TARGETS hints DESTINATION lib/kadu/modules)
