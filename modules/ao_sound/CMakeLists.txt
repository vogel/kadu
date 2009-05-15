set (SOURCES
	ao_sound.cpp)

set (MOC_SOURCES
	ao_sound.h)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (ao_sound.desc)
qt_wrap_cpp (ao_sound MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (AO REQUIRED ao)
include_directories (${AO_INCLUDE_DIRS})
link_directories (${AO_LIBRARY_DIRS})

add_library (ao_sound ${ao_sound} ${SOURCES} ${MOC_FILES})
target_link_libraries(ao_sound ${AO_LIBRARIES})

if (ao_sound STREQUAL STATIC)
	set_target_properties(ao_sound PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (ao_sound STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(ao_sound kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(ao_sound PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS ao_sound DESTINATION ${MODULEDIR})
endif (ao_sound STREQUAL STATIC)

