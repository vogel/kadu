set (SOURCES
	osd_hint.cpp
	osd_hint_manager.cpp
	osd_hints_configuration_widget.cpp
	osd_hints.cpp
)

set (MOC_SOURCES
	osd_hint.h
	osd_hint_manager.h
	osd_hints_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/osd_hints_pl.ts
)

set (CONFIGURATION_FILES
	configuration/osd_hints.ui)

set (DATA_FILES
	data/blocking22.png
	data/blocking32.png
	data/blocking64.png
	data/busy22.png
	data/busy32.png
	data/busy64.png
	data/busywithdescription22.png
	data/busywithdescription32.png
	data/busywithdescription64.png
	data/criticalsmall22.png
	data/criticalsmall32.png
	data/criticalsmall64.png
	data/dnd22.png
	data/dnd32.png
	data/dnd64.png
	data/dndwithdescription22.png
	data/dndwithdescription32.png
	data/dndwithdescription64.png
	data/ffc22.png
	data/ffc32.png
	data/ffc64.png
	data/ffcwithdescription22.png
	data/ffcwithdescription32.png
	data/ffcwithdescription64.png
	data/invisible22.png
	data/invisible32.png
	data/invisible64.png
	data/invisiblewithdescription32.png
	data/invisiblewithdescriptiond22.png
	data/invisiblewithdescriptiond64.png
	data/License
	data/managekeyswindowicon22.png
	data/managekeyswindowicon32.png
	data/managekeyswindowicon64.png
	data/mediaplayer22.png
	data/mediaplayer32.png
	data/mediaplayer64.png
	data/message22.png
	data/message32.png
	data/message64.png
	data/offline22.png
	data/offline32.png
	data/offline64.png
	data/offline_alt22.png
	data/offline_alt32.png
	data/offline_alt64.png
	data/offlinewithdescription22.png
	data/offlinewithdescription32.png
	data/offlinewithdescription64.png
	data/offlinewithdescription_alt22.png
	data/offlinewithdescription_alt32.png
	data/offlinewithdescription_alt64.png
	data/online22.png
	data/online32.png
	data/online64.png
	data/onlinewithdescription22.png
	data/onlinewithdescription32.png
	data/onlinewithdescription64.png
	data/osd_icon_alt.png
	data/osd_icon.png
	data/sendfile22.png
	data/sendfile32.png
	data/sendfile64.png
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (osd_hints.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (osd_hints MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/osd_hints)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (osd_hints ${osd_hints} ${SOURCES} ${MOC_FILES})
add_custom_target (osd_hints-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (osd_hints osd_hints-translations)
if (osd_hints STREQUAL STATIC)
	set_target_properties(osd_hints PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (osd_hints STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(osd_hints kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(osd_hints PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS osd_hints DESTINATION ${MODULEDIR})
endif (osd_hints STREQUAL STATIC)
