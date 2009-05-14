set (SOURCES
	gg_avatars.cpp)

set (MOC_SOURCES
	gg_avatars.h)

set (TRANSLATION_SOURCES
	translations/gg_avatars_pl.ts)

set (CHAT_FILES
	data/chat/gg_avatars.syntax
)

set (INFOPANEL_FILES
	data/infopanel/gg_avatars.syntax
)

install (FILES ${CHAT_FILES}
	DESTINATION share/kadu/syntax/chat)

install (FILES ${INFOPANEL_FILES}
	DESTINATION share/kadu/syntax/infopanel)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (gg_avatars.desc)
#kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (gg_avatars MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (gg_avatars ${gg_avatars} ${SOURCES} ${MOC_FILES})
add_custom_target (gg_avatars-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (gg_avatars gg_avatars-translations)
if (gg_avatars STREQUAL STATIC)
	set_target_properties(gg_avatars PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (gg_avatars STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(gg_avatars kadu_core notify history)
	endif (WIN32)
	if (APPLE)
		set_target_properties(gg_avatars PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS gg_avatars DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (gg_avatars STREQUAL STATIC)

