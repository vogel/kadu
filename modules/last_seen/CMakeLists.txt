set (SOURCES
	infos.cpp
	infos_dialog.cpp)

set (MOC_SOURCES
	infos.h
	infos_dialog.h)

set (TRANSLATION_SOURCES
	translations/last_seen_pl.ts)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (last_seen.desc)
qt_wrap_cpp (last_seen MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (last_seen ${last_seen} ${SOURCES} ${MOC_FILES})
add_custom_target (last_seen-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (last_seen last_seen-translations)
if (last_seen STREQUAL STATIC)
	set_target_properties(last_seen PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (last_seen STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(last_seen kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(last_seen PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS last_seen DESTINATION lib/kadu/modules)
endif (last_seen STREQUAL STATIC)

