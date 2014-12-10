project (encryption_otr)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

include (FindPkgConfig)

set (SOURCES
	gui/widgets/otr-account-configuration-widget-factory.cpp
	gui/widgets/otr-account-configuration-widget.cpp
	gui/widgets/otr-buddy-configuration-widget-factory.cpp
	gui/widgets/otr-buddy-configuration-widget.cpp
	gui/widgets/otr-chat-top-bar-widget-factory.cpp
	gui/widgets/otr-chat-top-bar-widget.cpp
	gui/widgets/otr-peer-identity-verification-fingerprint-exchange-page.cpp
	gui/widgets/otr-peer-identity-verification-progress-page.cpp
	gui/widgets/otr-peer-identity-verification-question-and-answer-page.cpp
	gui/widgets/otr-peer-identity-verification-respond-question-and-answer-page.cpp
	gui/widgets/otr-peer-identity-verification-respond-shared-secret-page.cpp
	gui/widgets/otr-peer-identity-verification-result-page.cpp
	gui/widgets/otr-peer-identity-verification-select-method-page.cpp
	gui/widgets/otr-peer-identity-verification-shared-secret-page.cpp

	gui/windows/otr-peer-identity-verification-window-factory.cpp
	gui/windows/otr-peer-identity-verification-window-repository.cpp
	gui/windows/otr-peer-identity-verification-window.cpp

	otr-app-ops-service.cpp
	otr-context-converter.cpp
	otr-create-private-key-job.cpp
	otr-create-private-key-worker.cpp
	otr-error-message-service.cpp
	otr-fingerprint-service.cpp
	otr-instance-tag-service.cpp
	otr-is-logged-in-service.cpp
	otr-message-event-service.cpp
	otr-message-service.cpp
	otr-module.cpp
	otr-notifier.cpp
	otr-op-data.cpp
	otr-op-data-factory.cpp
	otr-peer-identity-verification-service.cpp
	otr-peer-identity-verification-state.cpp
	otr-plugin.cpp
	otr-policy.cpp
	otr-policy-service.cpp
	otr-private-key-service.cpp
	otr-path-service.cpp
	otr-raw-message-transformer.cpp
	otr-session-service.cpp
	otr-timer-service.cpp
	otr-trust-level-service.cpp
	otr-user-state-service.cpp
)

set (MOC_SOURCES
	gui/widgets/otr-account-configuration-widget.h
	gui/widgets/otr-buddy-configuration-widget.h
	gui/widgets/otr-chat-top-bar-widget-factory.h
	gui/widgets/otr-chat-top-bar-widget.h
	gui/widgets/otr-peer-identity-verification-fingerprint-exchange-page.h
	gui/widgets/otr-peer-identity-verification-progress-page.h
	gui/widgets/otr-peer-identity-verification-question-and-answer-page.h
	gui/widgets/otr-peer-identity-verification-respond-question-and-answer-page.h
	gui/widgets/otr-peer-identity-verification-select-method-page.h
	gui/widgets/otr-peer-identity-verification-result-page.h
	gui/widgets/otr-peer-identity-verification-respond-shared-secret-page.h
	gui/widgets/otr-peer-identity-verification-shared-secret-page.h

	gui/windows/otr-peer-identity-verification-window-factory.h
	gui/windows/otr-peer-identity-verification-window-repository.h
	gui/windows/otr-peer-identity-verification-window.h

	otr-app-ops-service.h
	otr-context-converter.h
	otr-create-private-key-job.h
	otr-create-private-key-worker.h
	otr-error-message-service.h
	otr-fingerprint-service.h
	otr-instance-tag-service.h
	otr-is-logged-in-service.h
	otr-message-event-service.h
	otr-message-service.h
	otr-notifier.h
	otr-op-data-factory.h
	otr-path-service.h
	otr-peer-identity-verification-service.h
	otr-plugin.h
	otr-policy-service.h
	otr-private-key-service.h
	otr-raw-message-transformer.h
	otr-session-service.h
	otr-timer-service.h
	otr-trust-level-service.h
	otr-user-state-service.h
)

set (CONFIGURATION_FILES
)

pkg_check_modules (LIBOTR REQUIRED libotr>=4.0 REQUIRED)

set (LIBRARIES ${QCA2_LIBRARIES} ${LIBOTR_LDFLAGS} gcrypt)

if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	include_directories (${LIBOTR_INCLUDE_DIRS})
endif ()

kadu_plugin (encryption_otr
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
