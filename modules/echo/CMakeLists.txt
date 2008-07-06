set (SOURCES
	echo.cpp
)

set (MOC_SOURCES
	echo.h
)
qt_wrap_cpp (echo MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (echo ${echo} ${SOURCES} ${MOC_FILES})
add_custom_target (echo-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (echo echo-translations)

install (TARGETS echo DESTINATION lib/kadu/modules)
