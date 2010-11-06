set (SOURCES
	autodownloader.cpp
	citysearchresult.cpp
	forecast_container.cpp
	get_forecast.cpp
	getcitydialog.cpp
	search_location_id.cpp
	show_forecast_dialog.cpp
	showforecastframe.cpp
	textprogress.cpp
	weather.cpp
	weather_cfg_ui_handler.cpp
	weather_global.cpp
	weather_parser.cpp
)

set (MOC_SOURCES
	autodownloader.h
	get_forecast.h
	getcitydialog.h
	search_location_id.h
	show_forecast_dialog.h
	showforecastframe.h
	textprogress.h
	weather.h
	weather_cfg_ui_handler.h
)

set (TRANSLATION_SOURCES
	translations/weather_pl.ts
)

set (CONFIGURATION_FILES
	configuration/weather.ui
)

set (DATA_FILES
	data/interia.ini
	data/onetweather.ini
	data/pfweather.ini
)

set (ICON_FILES
	data/icons/01.gif
	data/icons/02.gif
	data/icons/03.gif
	data/icons/04.gif
	data/icons/05.gif
	data/icons/r01.gif
	data/icons/r02.gif
	data/icons/r03.gif
	data/icons/r04.gif
	data/icons/rr01.gif
	data/icons/rr02.gif
	data/icons/rr03.gif
	data/icons/rr04.gif
	data/icons/rs01.gif
	data/icons/rs02.gif
	data/icons/rs03.gif
	data/icons/rs04.gif
	data/icons/ss01.gif
	data/icons/ss02.gif
	data/icons/ss03.gif
	data/icons/ss04.gif
	data/icons/t01.gif
	data/icons/t02.gif
	data/icons/t03.gif
	data/icons/t04.gif
	data/icons/t05.gif
	data/icons/t06.gif
	data/icons/t07.gif
	data/icons/t08.gif
	data/icons/t09.gif
	data/icons/t10.gif
)

if (WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif (WIN32)

kadu_module_desc (weather.desc)
kadu_module_configuration (${CONFIGURATION_FILES})
qt_wrap_cpp (weather MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations"
)

install (FILES ${DATA_FILES}
	DESTINATION "share/kadu/modules/data/weather"
)

install (FILES ${ICON_FILES}
	DESTINATION "share/kadu/modules/data/weather/icons"
)

if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (weather ${weather} ${SOURCES} ${MOC_FILES})
add_custom_target (weather-translations DEPENDS ${TRANSLATION_FILES})
if (weather STREQUAL STATIC)
	set_target_properties (weather PROPERTIES COMPILE_FLAGS "-DKADULIB")
else (weather STREQUAL STATIC)
	if (WIN32)
		target_link_libraries (weather kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties (weather PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (weather STREQUAL STATIC)
add_dependencies (weather weather-translations)

install (TARGETS weather DESTINATION ${MODULEDIR})
