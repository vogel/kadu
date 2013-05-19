project (encryption_ng_otr)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	gui/widgets/encryption-ng-otr-account-configuration-widget-factory.cpp
	gui/widgets/encryption-ng-otr-account-configuration-widget.cpp

	encryption-ng-otr-app-ops-wrapper.cpp
	encryption-ng-otr-notifier.cpp
	encryption-ng-otr-op-data.cpp
	encryption-ng-otr-plugin.cpp
	encryption-ng-otr-policy.cpp
	encryption-ng-otr-policy-account-store.cpp
	encryption-ng-otr-private-key-service.cpp
	encryption-ng-otr-raw-message-transformer.cpp
	encryption-ng-otr-user-state.cpp
)

set (MOC_SOURCES
	gui/widgets/encryption-ng-otr-account-configuration-widget.h

	encryption-ng-otr-app-ops-wrapper.h
	encryption-ng-otr-notifier.h
	encryption-ng-otr-plugin.h
	encryption-ng-otr-private-key-service.h
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
