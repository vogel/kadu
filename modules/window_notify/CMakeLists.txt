set (SOURCES
	window_notify.cpp
)

set (MOC_SOURCES
	window_notify.h
)

set (TRANSLATION_SOURCES
	translations/window_notify_de.ts
	translations/window_notify_fr.ts
	translations/window_notify_it.ts
	translations/window_notify_pl.ts
)

kadu_module (window_notify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
