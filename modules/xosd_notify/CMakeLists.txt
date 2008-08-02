set (xosd_notify_SRCS
	xosd_notify.cpp
)

set (xosd_notify_MOC_SRCS
	xosd_notify.h
)

set (CONFIGURATION_FILES
	configuration/xosd_notify.ui)

kadu_module_configuration (${CONFIGURATION_FILES})
kadu_module_desc (xosd_notify.desc)
qt_wrap_cpp (xosd_notify MOC_FILES ${xosd_notify_MOC_SRCS})

set (TRANSLATION_SOURCES
	translations/xosd_notify_de.ts
	translations/xosd_notify_fr.ts
	translations/xosd_notify_it.ts
	translations/xosd_notify_pl.ts
)

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (xosd_notify ${xosd_notify} ${xosd_notify_SRCS} ${MOC_FILES})
add_custom_target (xosd_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (xosd_notify xosd_notify-translations)
