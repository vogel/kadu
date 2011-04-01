set (SOURCES
	exec_notify.cpp
)

set (MOC_SOURCES
	exec_notify.h
)

set (TRANSLATION_SOURCES
	translations/exec_notify_en.ts
	translations/exec_notify_de.ts
	translations/exec_notify_fr.ts
	translations/exec_notify_it.ts
	translations/exec_notify_pl.ts
)

kadu_module (exec_notify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
