set (SOURCES
	sql_history.cpp
	history_dialog.cpp
	search_dialog.cpp
)

set (MOC_SOURCES
	sql_history.h
	history_dialog.h
	search_dialog.h
)

set (TRANSLATION_SOURCES
	translations/sql_history_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sql_history.ui
	configuration/history-advanced.ui
)

kadu_module_desc (sql_history.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (sql_history MOC_FILES ${MOC_SOURCES})
add_definitions (-DQT_QTSQL)
qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (sql_history ${history} ${SOURCES} ${MOC_FILES})
add_custom_target (sql_history-translations DEPENDS ${TRANSLATION_FILES})
if (sql_history STREQUAL STATIC)
	set_target_properties(sql_history PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (sql_history STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(sql_history kadu_core)
	endif (WIN32)
endif (sql_history STREQUAL STATIC)
add_dependencies (sql_history sql_history-translations)

install (TARGETS sql_history DESTINATION lib/kadu/modules)
