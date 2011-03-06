set (SOURCES
	configuration/encryption-ng-configuration.cpp

	keys/keys-manager.cpp
	keys/key-shared.cpp
	keys/key.cpp

	notify/encryption-ng-notification.cpp

	decryptor.cpp
	decryptor-wrapper.cpp
	encryption-actions.cpp
	encryption-chat-data.cpp
	encryption-manager.cpp
	encryption-ng-configuration-ui-handler.cpp
	encryption-ng-module.cpp
	encryption-provider-manager.cpp
	encryptor.cpp
)

set (MOC_SOURCES
	keys/keys-manager.h
	keys/key-shared.h

	notify/encryption-ng-notification.h

	decryptor.h
	decryptor-wrapper.h
	encryption-actions.h
	encryption-chat-data.h
	encryption-manager.h
	encryption-ng-configuration-ui-handler.h
	encryption-provider.h
	encryption-provider-manager.h
	encryptor.h
)

set (TRANSLATION_SOURCES
	translations/encryption_ng_de.ts
	translations/encryption_ng_fr.ts
	translations/encryption_ng_it.ts
	translations/encryption_ng_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/encryption-ng.ui
)

if (MINGW)
	if (NOT MINGW_QCA_DIR)
		message (SEND_ERROR " Pass path to libqca2 -DMINGW_QCA_DIR=path")
	else (NOT MINGW_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${MINGW_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARY_DIRS ${MINGW_QCA_DIR}/bin)
		set (QCA2_LIBRARIES -lqca2)
	endif (NOT MINGW_QCA_DIR)
else (MINGW)
	pkg_search_module (QCA2 REQUIRED qca2)
endif (MINGW)
if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARY_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/)
		set (QCA2_LIBRARIES "-framework qca")
	endif (QCA_FRAMEWORK_DIR)
endif (APPLE)

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

kadu_module (encryption_ng
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${QCA2_LIBRARIES}
	MODULE_BUILDDEF ENCRYPTION_BUILD
)
