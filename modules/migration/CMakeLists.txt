set (SOURCES
	migration.cpp
)

set (MOC_SOURCES
	migration.h
)

set (TRANSLATION_SOURCES
	translations/migration_de.ts
	translations/migration_fr.ts
	translations/migration_it.ts
	translations/migration_pl.ts
)

set (CONFIGURATION_FILES
	configuration/migration.ui)

kadu_module_desc (migration.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (migration MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (migration ${migration} ${SOURCES} ${MOC_FILES})
add_custom_target (migration-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (migration migration-translations)

install (TARGETS migration DESTINATION lib/kadu/modules)
