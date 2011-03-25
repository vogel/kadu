set (SOURCES
	kadudbus.cpp
	kadurootobject.cpp
)

set (MOC_SOURCES
	kadudbus.h
	kadurootobject.h
)

kadu_plugin (dbus
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	#PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	#PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

target_link_libraries (dbus ${QT_QTDBUS_LIBRARIES})
