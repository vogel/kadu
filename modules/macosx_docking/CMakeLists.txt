set (SOURCES
	macosx_docking.cpp
)

set (MOC_SOURCES
	macosx_docking.h
)

set (CONFIGURATION_FILES
	configuration/macosx_docking.ui)

kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (macosx_docking MOC_FILES ${MOC_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (macosx_docking ${history} ${SOURCES} ${MOC_FILES})
add_custom_target (macosx_docking-translations DEPENDS ${TRANSLATION_FILES})
if (macosx_docking STREQUAL STATIC)
	set_target_properties(macosx_docking PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (macosx_docking STREQUAL STATIC)
	if (APPLE)
		set_target_properties(macosx_docking PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (macosx_docking STREQUAL STATIC)
add_dependencies (macosx_docking macosx_docking-translations)

install (TARGETS macosx_docking DESTINATION lib/kadu/modules)
