project (sms)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (QT_USE_QTXML 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
set (QT_USE_QTSCRIPT 1)
# Only needed for script debugger
#set (QT_USE_QTSCRIPTTOOLS 1)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	gui/windows/sms-dialog.cpp
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
	PLUGIN_DATA_DIRECTORY ${DATA_DIRECTORY}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QT_LIBRARIES}
	PLUGIN_BUILDDEF SMS_BUILD
	PLUGIN_DEPENDENCIES history
)
