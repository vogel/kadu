set (SOURCES
	gui/windows/sms-dialog.cpp
	gui/windows/sms-image-dialog.cpp
	gui/windows/sms-progress-window.cpp
	gui/windows/sms-token-reader.cpp
	scripts/network-access-manager-wrapper.cpp
	scripts/network-reply-wrapper.cpp
	scripts/sms-script-manager.cpp

	mobile-number.cpp
	mobile-number-manager.cpp
	sms-actions.cpp
	sms-configuration-ui-handler.cpp
	sms-gateway-manager.cpp
	sms-gateway-query.cpp
	sms-external-sender.cpp
	sms-internal-sender.cpp
	sms-module.cpp
	sms-sender.cpp
)

set (MOC_SOURCES
	gui/windows/sms-dialog.h
	gui/windows/sms-image-dialog.h
	gui/windows/sms-progress-window.h
	gui/windows/sms-token-reader.h
	scripts/network-access-manager-wrapper.h
	scripts/network-reply-wrapper.h
	scripts/sms-script-manager.h

	sms-actions.h
	sms-configuration-ui-handler.h
	sms-gateway-query.h
	sms-external-sender.h
	sms-internal-sender.h
	sms-sender.h
)

set (DATA_DIRECTORY
	data/scripts
)

set (TRANSLATION_SOURCES
	translations/sms_de.ts
	translations/sms_fr.ts
	translations/sms_it.ts
	translations/sms_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sms.ui)

set (QT_MIN_VERSION "4.5.0")
find_package (Qt4 REQUIRED)
set (QT_USE_QTXML 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
set (QT_USE_QTSCRIPT 1)
set (QT_USE_QTSCRIPTTOOLS 1)
include (${QT_USE_FILE})

kadu_module (sms
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_DATA_DIRECTORY ${DATA_DIRECTORY}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF SMS_BUILD
	MODULE_DEPENDENCIES history
)

target_link_libraries (sms ${QT_LIBRARIES} ${QT_QTSCRIPTTOOLS_LIBRARY})
