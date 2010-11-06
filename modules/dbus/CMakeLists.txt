set (SOURCES
	kadudbus.cpp
	kadurootobject.cpp
)

set (MOC_SOURCES
	kadudbus.h
	kadurootobject.h
)

kadu_module (dbus
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	#MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	#MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

target_link_libraries (dbus ${QT_QTDBUS_LIBRARIES})
