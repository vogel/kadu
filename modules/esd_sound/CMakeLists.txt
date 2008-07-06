set (SOURCES
	esd_sound.cpp
)

set (MOC_SOURCES
	esd_sound.h
)

qt_wrap_cpp (esd_sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (esd_sound ${esd_sound} ${SOURCES} ${MOC_FILES})
add_custom_target (esd_sound-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (esd_sound esd_sound-translations)

install (TARGETS esd_sound DESTINATION lib/kadu/modules)
