set (SOURCES
	encryption-actions.cpp
	encryption-ng-module.cpp
	encryption-provider-manager.cpp
)

set (MOC_SOURCES
	encryption-actions.h
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

pkg_search_module (QCA2 REQUIRED qca2)
include_directories (${QCA2_INCLUDE_DIRS})
link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})

target_link_libraries (encryption_ng ${QCA2_LIBRARIES})
set_target_properties (encryption_ng PROPERTIES LINK_FLAGS "${QCA2_LDFLAGS_OTHER}")
