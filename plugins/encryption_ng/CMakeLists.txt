project (encryption_ng)

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

include (FindPkgConfig)

kadu_api_directories (plugins/encryption_ng
	configuration
	keys
	notify
	.
)

set (SOURCES
	actions/enable-encryption-action-description.cpp
	actions/generate-keys-action-description.cpp
	actions/send-public-key-action-description.cpp

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
	encryption-ng-plugin.cpp
	encryption-provider-manager.cpp
	encryptor.cpp
)

set (MOC_SOURCES
	actions/enable-encryption-action-description.h
	actions/generate-keys-action-description.h
	actions/send-public-key-action-description.h

	keys/keys-manager.h
	keys/key-shared.h

	notify/encryption-ng-notification.h

	decryptor.h
	decryptor-wrapper.h
	encryption-actions.h
	encryption-chat-data.h
	encryption-manager.h
	encryption-ng-configuration-ui-handler.h
	encryption-ng-plugin.h
	encryption-provider.h
	encryption-provider-manager.h
	encryptor.h
)

set (CONFIGURATION_FILES
	data/configuration/encryption-ng.ui
)

if (WIN32)
	if (NOT WIN_QCA_DIR)
		message (SEND_ERROR "Pass path to libqca2 -DWIN_QCA_DIR=path")
	else ()
		set (QCA2_INCLUDE_DIRS ${WIN_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARIES optimized qca2 debug qcad2)

		if (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/lib)
		else ()
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/bin)
		endif ()
	endif ()
else ()
	pkg_search_module (QCA2 REQUIRED qca2)
endif ()

if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARY_DIRS ${QCA_FRAMEWORK_DIR}/qca.framework/)
		set (QCA2_LIBRARIES "-framework qca")
	endif ()
endif ()

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

kadu_plugin (encryption_ng
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES}
	PLUGIN_BUILDDEF ENCRYPTION_BUILD
)
