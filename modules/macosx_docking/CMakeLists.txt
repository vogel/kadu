set (SOURCES
	macosx_docking.cpp
)

set (MOC_SOURCES
	macosx_docking.h
)

set (CONFIGURATION_FILES
	configuration/macosx_docking.ui)

set (TRANSLATION_SOURCES
	translations/macosx_docking_de.ts
	translations/macosx_docking_en.ts
	translations/macosx_docking_fr.ts
	translations/macosx_docking_it.ts
	translations/macosx_docking_pl.ts
)

kadu_module_desc (macosx_docking.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (macosx_docking MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})
install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (macosx_docking ${macosx_docking} ${SOURCES} ${MOC_FILES})
add_custom_target (macosx_docking-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (macosx_docking macosx_docking-translations)
if (macosx_docking STREQUAL STATIC)
	set_target_properties(macosx_docking PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (macosx_docking STREQUAL STATIC)
	if (APPLE)
		set_target_properties(macosx_docking PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS macosx_docking DESTINATION lib/kadu/modules)
endif (macosx_docking STREQUAL STATIC)

