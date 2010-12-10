set (SOURCES
	encryption-ng-rsa-module.cpp
)

set (MOC_SOURCES
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
)

kadu_module (encryption_ng_rsa
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (QCA2 REQUIRED qca2)
include_directories (${QCA2_INCLUDE_DIRS})
link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})

target_link_libraries (encryption_ng_rsa ${QCA2_LIBRARIES})
set_target_properties (encryption_ng_rsa PROPERTIES LINK_FLAGS "${QCA2_LDFLAGS_OTHER}")
