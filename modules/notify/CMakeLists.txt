set (SOURCES
	connection_error_notification.cpp
	new_message_notification.cpp
	notification.cpp
	notify.cpp
	protocol_notification.cpp
	status_changed_notification.cpp
)

set (MOC_SOURCES
	connection_error_notification.h
	new_message_notification.h
	notification.h
	notify.h
	protocol_notification.h
	status_changed_notification.h
)

set (TRANSLATION_SOURCES
	translations/notify_de.ts
	translations/notify_fr.ts
	translations/notify_it.ts
	translations/notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/notify.ui)

kadu_module_desc (notify.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (notify ${notify} ${SOURCES} ${MOC_FILES})
set_target_properties(notify PROPERTIES COMPILE_FLAGS "-DNOTIFY_BUILD" )
add_custom_target (notify-translations DEPENDS ${TRANSLATION_FILES})
if (notify STREQUAL STATIC)
set_target_properties(notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (notify STREQUAL STATIC)
if (WIN32)
target_link_libraries(notify kadu_core)
endif (WIN32)
endif (notify STREQUAL STATIC)
add_dependencies (notify notify-translations)

install (TARGETS notify DESTINATION lib/kadu/modules)
