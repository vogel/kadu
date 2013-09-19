project (otr)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED)

include (FindPkgConfig)

set (SOURCES
	gui/widgets/otr-account-configuration-widget-factory.cpp
	gui/widgets/otr-account-configuration-widget.cpp
	gui/widgets/otr-chat-top-bar-widget-factory.cpp
	gui/widgets/otr-chat-top-bar-widget.cpp
	gui/widgets/otr-peer-identity-verification-fingerprint-exchange-page.cpp
	gui/widgets/otr-peer-identity-verification-select-method-page.cpp

	gui/windows/otr-peer-identity-verification-window-factory.cpp
	gui/windows/otr-peer-identity-verification-window.cpp

	otr-app-ops-wrapper.cpp
	otr-context-converter.cpp
	otr-create-private-key-job.cpp
	otr-create-private-key-worker.cpp
	otr-fingerprint-service.cpp
	otr-notifier.cpp
	otr-op-data.cpp
	otr-peer-identity-verifier.cpp
	otr-plugin.cpp
	otr-policy.cpp
	otr-policy-account-store.cpp
	otr-private-key-service.cpp
	otr-raw-message-transformer.cpp
	otr-timer.cpp
	otr-trust-level-service.cpp
	otr-user-state.cpp
)

set (MOC_SOURCES
	gui/widgets/otr-account-configuration-widget.h
	gui/widgets/otr-chat-top-bar-widget-factory.h
	gui/widgets/otr-chat-top-bar-widget.h
	gui/widgets/otr-peer-identity-verification-fingerprint-exchange-page.h
	gui/widgets/otr-peer-identity-verification-select-method-page.h

	gui/windows/otr-peer-identity-verification-window-factory.h
	gui/windows/otr-peer-identity-verification-window.h

	otr-app-ops-wrapper.h
	otr-context-converter.h
	otr-create-private-key-job.h
	otr-create-private-key-worker.h
	otr-fingerprint-service.h
	otr-notifier.h
	otr-peer-identity-verifier.h
	otr-plugin.h
	otr-private-key-service.h
	otr-timer.h
	otr-trust-level-service.h
)

set (CONFIGURATION_FILES
)

pkg_check_modules (LIBOTR REQUIRED libotr>=4.0 REQUIRED)

set (LIBRARIES ${QCA2_LIBRARIES} ${LIBOTR_LDFLAGS} gcrypt)

kadu_plugin (otr
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_DEPENDENCIES encryption_ng
)
