set (SOURCES
	echo.cpp
	echo-module.cpp
)

set (MOC_SOURCES
	echo.h
)

set (TRANSLATION_SOURCES
	translations/echo_pl.ts
	translations/echo_en.ts
)


kadu_module (echo
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
)
