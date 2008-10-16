set (SOURCES
	qt4_docking_notify.cpp
)

set (MOC_SOURCES
	qt4_docking_notify.h
)

set (TRANSLATION_SOURCES
#	translations/qt4_docking_notify_pl.ts
)

kadu_module (qt4_docking_notify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
