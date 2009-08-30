set (SOURCES
	encryption.cpp
	keys_manager.cpp
	simlite.c
)

set (MOC_SOURCES
	encryption.h
	keys_manager.h
)

set (TRANSLATION_SOURCES
	translations/encryption_old_de.ts
	translations/encryption_old_fr.ts
	translations/encryption_old_it.ts
	translations/encryption_old_pl.ts
)

set (CONFIGURATION_FILES
        configuration/encryption_old.ui)


if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (encryption_old.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (encryption_old MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

if (MINGW)
  include_directories (${MINGW_OPENSSL_DIR}/include)
  link_directories (${MINGW_OPENSSL_DIR}/lib)
	set (OPENSSL_LIBRARIES -lcrypto)
else (MINGW)
  pkg_search_module (OPENSSL REQUIRED openssl)
  include_directories (${OPENSSL_INCLUDE_DIRS})
  link_directories (${OPENSSL_INCLUDE_DIRS})
endif (MINGW)

add_library (encryption_old ${encryption_old} ${SOURCES} ${MOC_FILES})
target_link_libraries(encryption_old ${OPENSSL_LIBRARIES})
add_custom_target (encryption_old-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (encryption_old encryption_old-translations)
if (encryption_old STREQUAL STATIC)
	set_target_properties(encryption_old PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (encryption_old STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(encryption_old kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(encryption_old PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS encryption_old DESTINATION ${MODULEDIR})
endif (encryption_old STREQUAL STATIC)

