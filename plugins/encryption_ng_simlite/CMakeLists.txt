project (encryption_ng_simlite)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
	encryption-ng-simlite-decryptor.cpp
	encryption-ng-simlite-encryptor.cpp
	encryption-ng-simlite-key-generator.cpp
	encryption-ng-simlite-key-importer.cpp
	encryption-ng-simlite-plugin.cpp
	encryption-ng-simlite-provider.cpp

	pkcs1_certificate.cpp
)

set (MOC_SOURCES
	encryption-ng-simlite-decryptor.h
	encryption-ng-simlite-encryptor.h
	encryption-ng-simlite-key-importer.h
	encryption-ng-simlite-plugin.h
	encryption-ng-simlite-provider.h
)

set (TRANSLATION_SOURCES
	translations/encryption_ng_simlite_cs.ts
	translations/encryption_ng_simlite_de.ts
	translations/encryption_ng_simlite_en.ts
	translations/encryption_ng_simlite_it.ts
	translations/encryption_ng_simlite_pl.ts
)

set (CONFIGURATION_FILES
)

if (WIN32)
	if (NOT WIN_QCA_DIR)
		message (SEND_ERROR "Pass path to libqca2 -DWIN_QCA_DIR=path")
	else (NOT WIN_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${WIN_QCA_DIR}/include/QtCrypto)

		if (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/lib)
		else (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/bin)
		endif (MSVC)

		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set (QCA2_LIBRARIES qcad2)
		else (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set (QCA2_LIBRARIES qca2)
		endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	endif (NOT WIN_QCA_DIR)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2)
endif (WIN32)

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

kadu_plugin (encryption_ng_simlite
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES}
	PLUGIN_DEPENDENCIES encryption_ng
)
