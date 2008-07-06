set (SOURCES
	exec_notify.cpp
)

set (MOC_SOURCES
	exec_notify.h
)

set (TRANSLATION_SOURCES
	translations/exec_notify_de.ts
	translations/exec_notify_ft.ts
	translations/exec_notify_it.ts
	translations/exec_notify_pl.ts
)

kadu_module_desc (exec_notify.desc)
qt_wrap_cpp (exec_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (exec_notify ${exec_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (exec_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (exec_notify exec_notify-translations)

install (TARGETS exec_notify DESTINATION lib/kadu/modules)
