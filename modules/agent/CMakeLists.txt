set (SOURCES
	agent.cpp
	agent_notifications.cpp
)

set (MOC_SOURCES
	agent.h
	agent_notifications.h
)

set (TRANSLATION_SOURCES
	translations/agent_pl.ts)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (agent.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (agent MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (agent ${agent} ${SOURCES} ${MOC_FILES})
add_custom_target (agent-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (agent agent-translations)
if (agent STREQUAL STATIC)
	set_target_properties(agent PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (agent STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(agent kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(agent PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS agent DESTINATION ${MODULEDIR})
endif (agent STREQUAL STATIC)

