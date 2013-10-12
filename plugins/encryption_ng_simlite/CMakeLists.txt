project (encryption_ng_simlite)

cmake_minimum_required (VERSION 2.8)

# Needed for FindQCA2.cmake when building externally
list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules")

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	actions/simlite-send-public-key-action-description.cpp

	configuration/encryption-ng-simlite-configuration.cpp

	encryption-ng-simlite-decryptor.cpp
	encryption-ng-simlite-encryptor.cpp
	encryption-ng-simlite-key-generator.cpp
	encryption-ng-simlite-key-importer.cpp
	encryption-ng-simlite-message-filter.cpp
	encryption-ng-simlite-plugin.cpp
	encryption-ng-simlite-provider.cpp

	pkcs1_certificate.cpp
)

set (MOC_SOURCES
	actions/simlite-send-public-key-action-description.h

	encryption-ng-simlite-decryptor.h
	encryption-ng-simlite-encryptor.h
	encryption-ng-simlite-key-importer.h
	encryption-ng-simlite-message-filter.h
	encryption-ng-simlite-plugin.h
	encryption-ng-simlite-provider.h
)

set (CONFIGURATION_FILES
	data/configuration/encryption-ng-simlite.ui
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

kadu_plugin (encryption_ng_simlite
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES}
	PLUGIN_DEPENDENCIES encryption_ng
)
