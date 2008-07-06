set (SOURCES
	nas_sound.cpp
)

set (MOC_SOURCES
	nas_sound.h
)

kadu_module_desc (nas_sound.desc)
qt_wrap_cpp (migration MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (nas_sound ${migration} ${SOURCES} ${MOC_FILES})
add_custom_target (nas_sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (nas_sound nas_sound-translations)

install (TARGETS nas_sound DESTINATION lib/kadu/modules)
