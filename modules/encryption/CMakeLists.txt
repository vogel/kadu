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
	translations/encryption_de.ts
	translations/encryption_fr.ts
	translations/encryption_it.ts
	translations/encryption_pl.ts
)

set (CONFIGURATION_FILES
        configuration/encryption.ui)


if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (encryption.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (encryption MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (OPENSSL REQUIRED openssl)
include_directories (${OPENSSL_INCLUDE_DIRS})
link_directories (${OPENSSL_INCLUDE_DIRS})

add_library (encryption ${encryption} ${SOURCES} ${MOC_FILES})
target_link_libraries(encryption ${OPENSSL_LIBRARIES})
add_custom_target (encryption-translations DEPENDS ${TRANSLATION_FILES})
if (encryption STREQUAL STATIC)
	set_target_properties(encryption PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (encryption STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(encryption kadu_core)
	endif (WIN32)
endif (encryption STREQUAL STATIC)
add_dependencies (encryption encryption-translations)

install (TARGETS encryption DESTINATION lib/kadu/modules)
