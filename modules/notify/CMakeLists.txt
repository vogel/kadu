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

kadu_module (notify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF NOTIFY_BUILD
)
