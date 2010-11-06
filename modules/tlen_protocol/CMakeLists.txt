#pkg_search_module (QCA2 REQUIRED qca2)
#pkg_search_module (IDN REQUIRED libidn)

set (SOURCES
	gui/widgets/tlen-add-account-widget.cpp
	gui/widgets/tlen-contact-widget.cpp
	gui/widgets/tlen-create-account-widget.cpp
	gui/widgets/tlen-edit-account-widget.cpp
	gui/widgets/tlen-personal-info-widget.cpp
	open-chat-with/tlen-open-chat-with-runner.cpp
	server/tlen-avatar-fetcher.cpp
	services/tlen-avatar-service.cpp
	services/tlen-chat-service.cpp
	services/tlen-personal-info-service.cpp
	tlen-account-data-manager.cpp
	tlen-account-details.cpp
	tlen-contact-details.cpp
	tlen-contact-manager.cpp
	tlen-protocol.cpp
	tlen-protocol-factory.cpp
	QtTlen/auth.cpp
#	QtTlen/filetransfer.cpp
#	QtTlen/roster_item.cpp
	QtTlen/sha1.cpp
	QtTlen/tlen.cpp
)

set (MOC_SOURCES
	gui/widgets/tlen-add-account-widget.h
	gui/widgets/tlen-contact-widget.h
	gui/widgets/tlen-create-account-widget.h
	gui/widgets/tlen-edit-account-widget.h
	gui/widgets/tlen-personal-info-widget.h
#	open-chat-with/tlen-open-chat-with-runner.h
	server/tlen-avatar-fetcher.h
	services/tlen-avatar-service.h
	services/tlen-chat-service.h
	services/tlen-personal-info-service.h
#	tlen-account-data-manager.h
#	tlen-account-details.h
#	tlen-contact-details.h
#	tlen-contact-manager.h
	tlen-protocol.h
#	tlen-protocol-factory.h
#	QtTlen/auth.h
#	QtTlen/filetransfer.h
#	QtTlen/roster_item.h
#	QtTlen/sha1.h
	QtTlen/tlen.h
)

set (TRANSLATION_SOURCES
#	translations/tlen_protocol_de.ts
#	translations/tlen_protocol_en.ts
#	translations/tlen_protocol_fr.ts
#	translations/tlen_protocol_it.ts
	translations/tlen_protocol_pl.ts
)

set (CONFIGURATION_FILES
	configuration/tlen-contact.ui
)

set (DATA_FILES
	data/status/away.png
	data/status/awayi.png
	data/status/chat.png
	data/status/chati.png
	data/status/dnd.png
	data/status/dndi.png
	data/status/invisible.png
	data/status/invisiblei.png
	data/status/offline.png
	data/status/offlinei.png
	data/status/online.png
	data/status/onlinei.png
	data/status/unavailable.png
	data/status/unavailablei.png
)

#link_directories (${QCA2_LIBRARY_DIRS} ${IDN_LIBRARY_DIRS})
#add_definitions (${QCA2_CFLAGS_OTHER} ${IDN_CFLAGS_OTHER})

kadu_module (tlen_protocol
	MODULE_SOURCES  ${SOURCES}
	MODULE_MOC_SOURCES  ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

#set_target_properties (tlen_protocol PROPERTIES
#	link_flags "${QCA2_LDFLAGS_OTHER} ${IDN_LDFLAGS_OTHER}"
#)

include_directories (
	./QtTlen
)

#target_link_libraries (tlen_protocol ${QCA2_LIBRARIES} ${IDN_LIBRARIES})
install (FILES ${DATA_FILES}
	DESTINATION ${KADU_MODULES_DIR}/data/tlen_protocol
)
