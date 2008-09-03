set (SOURCES
	echo.cpp
)

set (MOC_SOURCES
	echo.h
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

qt_wrap_cpp (echo MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (echo ${echo} ${SOURCES} ${MOC_FILES})
add_custom_target (echo-translations DEPENDS ${TRANSLATION_FILES})
if (echo STREQUAL STATIC)
	set_target_properties(echo PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (echo STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(echo kadu_core)
	endif (WIN32)
endif (echo STREQUAL STATIC)
add_dependencies (echo echo-translations)

install (TARGETS echo DESTINATION lib/kadu/modules)
