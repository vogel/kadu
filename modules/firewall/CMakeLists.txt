set (SOURCES
	firewall.cpp)

set (MOC_SOURCES
	firewall.h)

set (TRANSLATION_SOURCES
	translations/firewall_pl.ts)

set (CONFIGURATION_FILES
	configuration/firewall.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (firewall.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (firewall MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (firewall ${firewall} ${SOURCES} ${MOC_FILES})
add_custom_target (firewall-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (firewall firewall-translations)
if (firewall STREQUAL STATIC)
	set_target_properties(firewall PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (firewall STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(firewall kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(firewall PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS firewall DESTINATION lib/kadu/modules)
endif (firewall STREQUAL STATIC)

