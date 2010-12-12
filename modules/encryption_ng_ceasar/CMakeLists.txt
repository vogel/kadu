set (SOURCES
	encryption-ng-ceasar-decryptor.cpp
	encryption-ng-ceasar-encryptor.cpp
	encryption-ng-ceasar-module.cpp
	encryption-ng-ceasar-provider.cpp
)

set (MOC_SOURCES
	encryption-ng-ceasar-decryptor.h
	encryption-ng-ceasar-encryptor.h
	encryption-ng-ceasar-provider.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
)

kadu_module (encryption_ng_ceasar
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
