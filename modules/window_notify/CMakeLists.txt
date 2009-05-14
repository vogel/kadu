set (SOURCES
	window_notify.cpp
)

set (MOC_SOURCES
	window_notify.h
)

set (TRANSLATION_SOURCES
	translations/window_notify_de.ts
	translations/window_notify_fr.ts
	translations/window_notify_it.ts
	translations/window_notify_pl.ts
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (window_notify.desc)
qt_wrap_cpp (window_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (window_notify ${window_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (window_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (window_notify window_notify-translations)
if (window_notify STREQUAL STATIC)
	set_target_properties(window_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (window_notify STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(window_notify kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(pcspeaker PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS window_notify DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (window_notify STREQUAL STATIC)

