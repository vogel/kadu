project (encryption_ng)

cmake_minimum_required (VERSION 2.8)

# Needed for FindQCA2.cmake when building externally
list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules")

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/encryption_ng
	configuration
	keys
	notify
	.
)

set (SOURCES
	actions/encryption-set-up-action-description.cpp
	actions/generate-keys-action-description.cpp

	gui/encryption-set-up-menu.cpp

	keys/keys-manager.cpp
	keys/key-shared.cpp
	keys/key.cpp

	notify/encryption-ng-notification.cpp

	decryptor.cpp
	decryptor-wrapper.cpp
	encryption-actions.cpp
	encryption-chat-data.cpp
	encryption-manager.cpp
	encryption-ng-plugin.cpp
	encryption-provider-manager.cpp
	encryptor.cpp
)

set (MOC_SOURCES
	actions/encryption-set-up-action-description.h
	actions/generate-keys-action-description.h

	gui/encryption-set-up-menu.h

	keys/keys-manager.h
	keys/key-shared.h

	notify/encryption-ng-notification.h

	decryptor.h
	decryptor-wrapper.h
	encryption-actions.h
	encryption-chat-data.h
	encryption-manager.h
	encryption-ng-plugin.h
	encryption-provider.h
	encryption-provider-manager.h
	encryptor.h
)

set (CONFIGURATION_FILES
)

find_package (QCA2 REQUIRED)

if (APPLE)
	if (QCA_FRAMEWORK_DIR)
		set (QCA2_INCLUDE_DIR ${QCA_FRAMEWORK_DIR}/qca.framework/Headers/)
		set (QCA2_LIBRARIES "-framework qca")
		link_directories (${QCA_FRAMEWORK_DIR}/qca.framework/)
	endif ()
endif ()

add_definitions (${QCA2_DEFINITIONS})
include_directories (${QCA2_INCLUDE_DIR})

kadu_plugin (encryption_ng
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES}
)
