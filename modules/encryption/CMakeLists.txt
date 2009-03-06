set (SOURCES
	encryption.cpp
	kadu_encryption_factory.cpp
	kadu_encryption_rsa.cpp
	kadu_encryption_simlite.cpp
	keys_manager.cpp
	pkcs1_certificate.cpp
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

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (QCA2 REQUIRED qca2)
include_directories (${QCA2_INCLUDE_DIRS})
link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})

add_library (encryption ${encryption} ${SOURCES} ${MOC_FILES})
add_custom_target (encryption-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (encryption encryption-translations)

target_link_libraries(encryption ${QCA2_LIBRARIES})
set_target_properties(encryption PROPERTIES LINK_FLAGS "${QCA2_LDFLAGS_OTHER}")
if (encryption STREQUAL STATIC)
	set_target_properties(encryption PROPERTIES COMPILE_FLAGS "-DKADULIB")
else (encryption STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(encryption kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(encryption PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS encryption DESTINATION lib/kadu/modules)
endif (encryption STREQUAL STATIC)

