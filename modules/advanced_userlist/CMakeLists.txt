set (SOURCES
	advanced_userlist.cpp
	main.cpp
)

set (MOC_SOURCES
	advanced_userlist.h
)

set (TRANSLATION_SOURCES
	translations/advanced_userlist_de.ts
	translations/advanced_userlist_fr.ts
	translations/advanced_userlist_it.ts
	translations/advanced_userlist_pl.ts
)

set (CONFIGURATION_FILES
	configuration/advanced_userlist.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (advanced_userlist.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (advanced_userlist MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (advanced_userlist ${advanced_userlist} ${SOURCES} ${MOC_FILES})
add_custom_target (advanced_userlist-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (advanced_userlist advanced_userlist-translations)
if (advanced_userlist STREQUAL STATIC)
	set_target_properties(advanced_userlist PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (advanced_userlist STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(advanced_userlist kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(advanced_userlist PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS advanced_userlist DESTINATION lib/kadu/modules)
endif (advanced_userlist STREQUAL STATIC)



