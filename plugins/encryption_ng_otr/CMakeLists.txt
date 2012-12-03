project (encryption_ng_otr)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	encryption-ng-otr-plugin.cpp
)

set (MOC_SOURCES
	encryption-ng-otr-plugin.h
)

set (CONFIGURATION_FILES
)

pkg_check_modules (LIBOTR REQUIRED libotr>=3.0 REQUIRED)

kadu_plugin (encryption_ng_otr
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES}
	PLUGIN_DEPENDENCIES encryption_ng
)
