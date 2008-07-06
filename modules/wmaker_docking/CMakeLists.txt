set (SOURCES
	wmaker_docking.cpp
)

set (MOC_SOURCES
	wmaker_docking.h
)

set (TRANSLATION_SOURCES
	translations/wmaker_docking_de.ts
	translations/wmaker_docking_ft.ts
	translations/wmaker_docking_it.ts
	translations/wmaker_docking_pl.ts
)

set (CONFIGURATION_FILES
	configuration/wmaker_docking.ui)

kadu_module_desc (wmaker_docking.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (wmaker_docking MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (wmaker_docking ${wmaker_docking} ${SOURCES} ${MOC_FILES})
add_custom_target (wmaker_docking-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (wmaker_docking wmaker_docking-translations)

install (TARGETS wmaker_docking DESTINATION lib/kadu/modules)
