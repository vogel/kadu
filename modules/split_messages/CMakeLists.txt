set (SOURCES
	split.cpp
)

set (MOC_SOURCES
	split.h
)

set (CONFIGURATION_FILES
	configuration/split_messages.ui)

set (TRANSLATION_SOURCES
	translations/split_messages_pl.ts)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (split_messages.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (split_messages MOC_FILES ${MOC_SOURCES})
qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (split_messages ${split_messages} ${SOURCES} ${MOC_FILES})
add_custom_target (split_messages-translations DEPENDS ${TRANSLATION_FILES})
if (split_messages STREQUAL STATIC)
	set_target_properties(split_messages PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (split_messages STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(split_messages kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(split_messages PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (split_messages STREQUAL STATIC)
add_dependencies (split_messages split_messages-translations)

install (TARGETS split_messages DESTINATION lib/kadu/modules)
