set (SOURCES
	hint.cpp
	hint_manager.cpp
	hint-over-user-configuration-window.cpp
	hints_configuration_widget.cpp
	hints-configuration-window.cpp
	hints.cpp
	hints-configuration-ui-handler.cpp
)

set (MOC_SOURCES
	hint.h
	hint_manager.h
	hint-over-user-configuration-window.h
	hints_configuration_widget.h
	hints-configuration-window.h
	hints-configuration-ui-handler.h
)

set (TRANSLATION_SOURCES
	translations/hints_en.ts
	translations/hints_de.ts
	translations/hints_fr.ts
	translations/hints_it.ts
	translations/hints_pl.ts
)

set (CONFIGURATION_FILES
	configuration/hint-over-user.ui
	configuration/hints.ui
	configuration/hints-advanced.ui
	configuration/hints-notifier.ui
)

kadu_module (hints
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
