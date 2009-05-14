set (SOURCES
	exec_notify.cpp
)

set (MOC_SOURCES
	exec_notify.h
)

set (TRANSLATION_SOURCES
	translations/exec_notify_de.ts
	translations/exec_notify_fr.ts
	translations/exec_notify_it.ts
	translations/exec_notify_pl.ts
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (exec_notify.desc)
qt_wrap_cpp (exec_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (exec_notify ${exec_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (exec_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (exec_notify exec_notify-translations)
if (exec_notify STREQUAL STATIC)
	set_target_properties(exec_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (exec_notify STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(exec_notify kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(encryption PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS exec_notify DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (exec_notify STREQUAL STATIC)

