set (SOURCES
	qt4_docking_notify.cpp
	qt4_docking_notify_configuration_widget.cpp
)

set (MOC_SOURCES
	qt4_docking_notify.h
	qt4_docking_notify_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/qt4_docking_notify_pl.ts
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (qt4_docking_notify.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (qt4_docking_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (qt4_docking_notify ${qt4_docking_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (qt4_docking_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (qt4_docking_notify qt4_docking_notify-translations)
if (qt4_docking_notify STREQUAL STATIC)
	set_target_properties(qt4_docking_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (qt4_docking_notify STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(qt4_docking_notify kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(qt4_docking_notify PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS qt4_docking_notify DESTINATION lib/kadu/modules)
endif (qt4_docking_notify STREQUAL STATIC)
