set (SOURCES
	profiles.cpp)

set (MOC_SOURCES
	profiles.h)

set (TRANSLATION_SOURCES
	translations/profiles_pl.ts)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (profiles.desc)
qt_wrap_cpp (profiles MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (profiles ${profiles} ${SOURCES} ${MOC_FILES})
add_custom_target (profiles-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (profiles profiles-translations)
if (profiles STREQUAL STATIC)
	set_target_properties(profiles PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (profiles STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(profiles kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(profiles PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS profiles DESTINATION lib/kadu/modules)
endif (profiles STREQUAL STATIC)

