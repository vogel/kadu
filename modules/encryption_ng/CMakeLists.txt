set (SOURCES
	keys/keys-manager.cpp
	keys/key-shared.cpp
	keys/key.cpp

	decryptor-wrapper.cpp
	encryption-actions.cpp
	encryption-chat-data.cpp
	encryption-manager.cpp
	encryption-ng-module.cpp
	encryption-provider-manager.cpp
)

set (MOC_SOURCES
	keys/keys-manager.h
	keys/key-shared.h

	decryptor.h
	decryptor-wrapper.h
	encryption-actions.h
	encryption-chat-data.h
	encryption-manager.h
	encryption-provider.h
	encryption-provider-manager.h
	encryptor.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
)

kadu_module (encryption_ng
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
