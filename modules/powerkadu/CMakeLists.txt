set (SOURCES
	powerkadu.cpp
	about_dialog.cpp
)

set (MOC_SOURCES
	powerkadu.h
	about_dialog.h
)

set (TRANSLATION_SOURCES
	translations/powerkadu_pl.ts)

set (CONFIGURATION_FILES
	configuration/powerkadu.ui)

set (DATA_FILES
	data/AUTHORS
	data/AUTHORS.pl
	data/ChangeLog
	data/powerkadu_32x32.png
	data/powerkadu_big.png
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (powerkadu.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (powerkadu MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/powerkadu)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (powerkadu ${powerkadu} ${SOURCES} ${MOC_FILES})
add_custom_target (powerkadu-translations DEPENDS ${TRANSLATION_FILES})
if (powerkadu STREQUAL STATIC)
	set_target_properties(powerkadu PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (powerkadu STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(powerkadu kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(powerkadu PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (powerkadu STREQUAL STATIC)
add_dependencies (powerkadu powerkadu-translations)

install (TARGETS powerkadu DESTINATION ${MODULEDIR})
