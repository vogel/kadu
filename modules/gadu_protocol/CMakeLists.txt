set (SOURCES
	gadu.cpp
	gadu_account_data.cpp
	gadu_formatter.cpp
	gadu_images_manager.cpp
	gadu-private.cpp
	gadu_protocol_factory.cpp
	gadu_search.cpp
	gadu_status.cpp
	uins_list.cpp
)

set (MOC_SOURCES
	gadu.h
	gadu-private.h
)

set (TRANSLATION_SOURCES
)

kadu_module (gadu_protocol
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
)
