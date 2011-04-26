set (SOURCES
	echo.cpp
	echo-module.cpp
)

set (MOC_SOURCES
	echo.h
)

set (TRANSLATION_SOURCES
	translations/echo_en.ts
	translations/echo_pl.ts
)

kadu_plugin (echo
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
)
