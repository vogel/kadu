set (SOURCES
	encryption.cpp
	keys_manager.cpp
	simlite.c
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
	configuration/encryption.ui)

kadu_module (encryption
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (OPENSSL REQUIRED openssl)
include_directories (${OPENSSL_INCLUDE_DIRS})
link_directories (${OPENSSL_INCLUDE_DIRS})

target_link_libraries(encryption ${OPENSSL_LIBRARIES})
