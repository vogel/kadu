set (SOURCES
	account_management.cpp
	change_password.cpp
	register.cpp
	remind_password.cpp
	unregister.cpp
)

set (MOC_SOURCES
	account_management.h
	change_password.h
	register.h
	remind_password.h
	unregister.h
)

set (TRANSLATION_SOURCES
	translations/account_management_de.ts
	translations/account_management_fr.ts
	translations/account_management_it.ts
	translations/account_management_pl.ts
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (account_management.desc)
qt_wrap_cpp (account_management MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (account_management ${account_management} ${SOURCES} ${MOC_FILES})
add_custom_target (account_management-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (account_management account_management-translations)
if (account_management STREQUAL STATIC)
	set_target_properties(account_management PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (account_management STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(account_management kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(account_management PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS account_management DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (account_management STREQUAL STATIC)

