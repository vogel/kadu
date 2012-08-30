project (encryption_ng_simlite)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include (FindPkgConfig)

set (SOURCES
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
	encryption-ng-simlite-decryptor.h
	encryption-ng-simlite-encryptor.h
	encryption-ng-simlite-key-importer.h
	encryption-ng-simlite-message-filter.h
	encryption-ng-simlite-plugin.h
	encryption-ng-simlite-provider.h
)

set (CONFIGURATION_FILES
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

kadu_plugin (encryption_ng_simlite
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${QCA2_LIBRARIES}
	PLUGIN_DEPENDENCIES encryption_ng
)
