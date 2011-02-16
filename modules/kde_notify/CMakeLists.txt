set (QT_USE_QTDBUS 1)

set (SOURCES
	kde_notify.cpp
)

set (MOC_SOURCES
	kde_notify.h
)

set (TRANSLATION_SOURCES
	translations/kde_notify_de.ts
	translations/kde_notify_fr.ts
	translations/kde_notify_it.ts
	translations/kde_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/kde_notify.ui
)

kadu_module (kde_notify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${LIBGADU_LIBRARIES}
)

target_link_libraries (kde_notify ${QT_QTDBUS_LIBRARY})
