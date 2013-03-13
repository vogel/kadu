project (encryption_ng_otr)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	encryption-ng-otr-app-ops-wrapper.cpp
	encryption-ng-otr-message-filter.cpp
	encryption-ng-otr-plugin.cpp
	encryption-ng-otr-user-state-service.cpp
)

set (MOC_SOURCES
	encryption-ng-otr-app-ops-wrapper.h
	encryption-ng-otr-message-filter.h
	encryption-ng-otr-plugin.h
	encryption-ng-otr-user-state-service.h
)

set (CONFIGURATION_FILES
)

pkg_check_modules (LIBOTR REQUIRED libotr>=3.0 REQUIRED)

include_directories (${LIBOTR_INCLUDE_DIR})

kadu_plugin (encryption_ng_otr
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES} ${LIBOTR_LIBRARIES}
	PLUGIN_DEPENDENCIES encryption_ng
)
