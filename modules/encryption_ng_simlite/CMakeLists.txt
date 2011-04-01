set (SOURCES
	encryption-ng-simlite-decryptor.cpp
	encryption-ng-simlite-encryptor.cpp
	encryption-ng-simlite-key-generator.cpp
	encryption-ng-simlite-key-importer.cpp
	encryption-ng-simlite-module.cpp
	encryption-ng-simlite-provider.cpp

	pkcs1_certificate.cpp
)

set (MOC_SOURCES
	encryption-ng-simlite-decryptor.h
	encryption-ng-simlite-encryptor.h
	encryption-ng-simlite-key-importer.h
	encryption-ng-simlite-provider.h
)

set (TRANSLATION_SOURCES
	translations/encryption_ng_simlite_en.ts
	translations/encryption_ng_simlite_de.ts
	translations/encryption_ng_simlite_fr.ts
	translations/encryption_ng_simlite_it.ts
	translations/encryption_ng_simlite_pl.ts
)

set (CONFIGURATION_FILES
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

kadu_module (encryption_ng_simlite
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${QCA2_LIBRARIES}
	MODULE_DEPENDENCIES encryption_ng
)
