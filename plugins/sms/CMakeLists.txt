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

	mobile-number-manager.cpp
	mobile-number.cpp
	sms-actions.cpp
	sms-configuration-ui-handler.cpp
	sms-external-sender.cpp
	sms-gateway-manager.cpp
	sms-gateway-query.cpp
	sms-gateway.cpp
	sms-internal-sender.cpp
	sms-module.cpp
	sms-plugin-modules-factory.cpp
	sms-plugin-object.cpp
	sms-sender.cpp
	sms-token-read-job.cpp
)

set (DATA_DIRECTORY
	data/scripts
)

set (CONFIGURATION_FILES
	configuration/sms.ui
)

kadu_plugin (sms
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_DATA_DIRECTORIES ${DATA_DIRECTORY}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES history
	PLUGIN_ADDITIONAL_QT_MODULES Script
)
