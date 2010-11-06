set (SOURCES
	encryption.cpp
	kadu_encryption_factory.cpp
	kadu_encryption_rsa.cpp
	kadu_encryption_simlite.cpp
	keys_manager.cpp
	pkcs1_certificate.cpp
)

set (MOC_SOURCES
	encryption.h
	keys_manager.h
)

set (TRANSLATION_SOURCES
	translations/encryption_de.ts
	translations/encryption_fr.ts
	translations/encryption_it.ts
	translations/encryption_pl.ts
)

set (CONFIGURATION_FILES
	configuration/encryption.ui
)

kadu_module (encryption
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (QCA2 REQUIRED qca2)
include_directories (${QCA2_INCLUDE_DIRS})
link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})

target_link_libraries (encryption ${QCA2_LIBRARIES})
set_target_properties (encryption PROPERTIES LINK_FLAGS "${QCA2_LDFLAGS_OTHER}")
