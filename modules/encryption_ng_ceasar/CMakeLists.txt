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

if (MINGW)
	if (NOT MINGW_QCA_DIR)
		message (SEND_ERROR " Pass path to qca -DMINGW_QCA_DIR=path")
		set (MINGW_LIBSNDFILE_DIR "")
	else (NOT MINGW_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${MINGW_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARY_DIRS ${MINGW_QCA_DIR}/bin)
		set (QCA2_LIBRARIES -lqca2)
	endif (NOT MINGW_QCA_DIR)
else (MINGW)
	pkg_search_module (QCA2 REQUIRED qca2)
endif (MINGW)

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

kadu_module (encryption_ng_ceasar
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${QCA2_LIBRARIES}
)
