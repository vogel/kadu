project (sms)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	gui/windows/sms-dialog.cpp
	gui/windows/sms-dialog-repository.cpp
	scripts/network-access-manager-wrapper.cpp
	scripts/network-reply-wrapper.cpp
	scripts/sms-script-manager.cpp
	scripts/sms-translator.cpp

	mobile-number.cpp
	mobile-number-manager.cpp
	sms-actions.cpp
	sms-configuration-ui-handler.cpp
	sms-gateway.cpp
	sms-gateway-manager.cpp
	sms-gateway-query.cpp
	sms-external-sender.cpp
	sms-internal-sender.cpp
	sms-plugin.cpp
	sms-sender.cpp
	sms-token-read-job.cpp
)

set (MOC_SOURCES
	gui/windows/sms-dialog.h
	scripts/network-access-manager-wrapper.h
	scripts/network-reply-wrapper.h
	scripts/sms-script-manager.h
	scripts/sms-translator.h

	sms-actions.h
	sms-configuration-ui-handler.h
	sms-gateway-query.h
	sms-external-sender.h
	sms-internal-sender.h
	sms-plugin.h
	sms-sender.h
	sms-token-read-job.h
)

set (DATA_DIRECTORY
	data/scripts
)

set (CONFIGURATION_FILES
	configuration/sms.ui
)

kadu_plugin (sms
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DATA_DIRECTORIES ${DATA_DIRECTORY}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES history
	PLUGIN_ADDITIONAL_QT_MODULES Script
)
