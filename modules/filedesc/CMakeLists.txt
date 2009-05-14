set (SOURCES
	filedesc.cpp
)

set (MOC_SOURCES
	filedesc.h
)

set (TRANSLATION_SOURCES
	translations/filedesc_pl.ts
)

set (CONFIGURATION_FILES
	configuration/filedesc.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (filedesc.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (filedesc MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (filedesc ${filedesc} ${SOURCES} ${MOC_FILES})
add_custom_target (filedesc-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (filedesc filedesc-translations)
if (filedesc STREQUAL STATIC)
	set_target_properties(filedesc PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (filedesc STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(filedesc kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(filedesc PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS filedesc DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (filedesc STREQUAL STATIC)

