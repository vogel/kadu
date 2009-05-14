set (SOURCES
	qt4_docking.cpp
)

set (MOC_SOURCES
	qt4_docking.h
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (qt4_docking.desc)
qt_wrap_cpp (qt4_docking MOC_FILES ${MOC_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (qt4_docking ${qt4_docking} ${SOURCES} ${MOC_FILES})
add_custom_target (qt4_docking-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (qt4_docking qt4_docking-translations)
if (qt4_docking STREQUAL STATIC)
	set_target_properties(qt4_docking PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (qt4_docking STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(qt4_docking kadu_core docking)
	endif (WIN32)
	if (APPLE)
		set_target_properties(qt4_docking PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS qt4_docking DESTINATION lib${LIB_SUFFIX}/kadu/modules)
endif (qt4_docking STREQUAL STATIC)



