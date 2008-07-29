set (SOURCES
	history.cpp
	history_dialog.cpp
	history_module.cpp
	history_search_dialog.cpp
)

set (MOC_SOURCES
	history.h
	history_dialog.h
	history_module.h
	history_search_dialog.h
)

set (TRANSLATION_SOURCES
	translations/history_de.ts
	translations/history_fr.ts
	translations/history_it.ts
	translations/history_pl.ts
)

set (CONFIGURATION_FILES
	configuration/history.ui)

kadu_module_desc (history.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (history MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (history ${history} ${SOURCES} ${MOC_FILES})
set_target_properties(history PROPERTIES COMPILE_FLAGS "-DHISTORY_BUILD" )
add_custom_target (history-translations DEPENDS ${TRANSLATION_FILES})
if (history STREQUAL STATIC)
set_target_properties(history PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (history STREQUAL STATIC)
if (WIN32)
target_link_libraries(history kadu_core)
endif (WIN32)
endif (history STREQUAL STATIC)
add_dependencies (history history-translations)

install (TARGETS history DESTINATION lib/kadu/modules)
