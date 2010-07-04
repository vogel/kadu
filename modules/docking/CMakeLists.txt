if (APPLE)

set (SOURCES
	docking-module.cpp
	docking.cpp
	mac_docking_helper.cpp
)

set (MOC_SOURCES
	docking.h
	mac_docking_helper.h
)

else (APPLE)

set (SOURCES
	docking-module.cpp
	docking.cpp
)

set (MOC_SOURCES
	docking.h
)

endif (APPLE)

set (TRANSLATION_SOURCES
	translations/docking_de.ts
	translations/docking_fr.ts
	translations/docking_it.ts
	translations/docking_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/docking.ui)

kadu_module (docking
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF DOCKING_BUILD
)

if (WIN32)
set_target_properties(docking PROPERTIES COMPILE_FLAGS "-DDOCKING_BUILD")
endif (WIN32)
