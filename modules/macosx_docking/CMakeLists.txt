set (SOURCES
	macox_docking.cpp
)

set (MOC_SOURCES
	macox_docking.h
)

qt_wrap_cpp (macox_docking MOC_FILES ${MOC_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (macox_docking ${history} ${SOURCES} ${MOC_FILES})
add_custom_target (macox_docking-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (macox_docking macox_docking-translations)

install (TARGETS macox_docking DESTINATION lib/kadu/modules)
