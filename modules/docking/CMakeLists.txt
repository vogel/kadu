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
add_custom_target (docking-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (docking docking-translations)

install (TARGETS docking DESTINATION lib/kadu/modules)
