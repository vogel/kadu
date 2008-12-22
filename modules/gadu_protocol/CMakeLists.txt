set (SOURCES
	gadu.cpp
	gadu-account-data-manager.cpp
	gadu_account_data.cpp
	gadu_configuration_dialog.cpp
	gadu-contact-account-data.cpp
	gadu_formatter.cpp
	gadu_images_manager.cpp
	gadu-importer.cpp
	gadu-private.cpp
	gadu_protocol_factory.cpp
	gadu_search.cpp
	gadu_status.cpp
	uins_list.cpp
)

set (MOC_SOURCES
	gadu.h
	gadu_configuration_dialog.h
	gadu-contact-account-data.h
	gadu-importer.h
	gadu-private.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
	configuration/gadu_protocol.ui
)

kadu_module (gadu_protocol
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
#	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
