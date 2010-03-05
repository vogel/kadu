set (SOURCES
	cenzor.cpp)

set (MOC_SOURCES
	cenzor.h)

set (TRANSLATION_SOURCES
	translations/cenzor_pl.ts)

set (CONFIGURATION_FILES
	configuration/cenzor.ui)

set (DATA_FILES
	data/cenzor_words.conf
	data/cenzor_words_ok.conf)

kadu_module (cenzor
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/cenzor)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (cenzor ${cenzor} ${SOURCES} ${MOC_FILES})
add_custom_target (cenzor-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (cenzor cenzor-translations)
if (cenzor STREQUAL STATIC)
	set_target_properties(cenzor PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (cenzor STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(cenzor kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(cenzor PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS cenzor DESTINATION lib/kadu/modules)
endif (cenzor STREQUAL STATIC)

