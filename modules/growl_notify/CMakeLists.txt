set (SOURCES
	growl_notify.cpp
	growl_notify_configuration_widget.cpp
	growlnotifier.cpp
)

set (MOC_SOURCES
	growl_notify.h
	growl_notify_configuration_widget.h
	growlnotifier.h
)

set (TRANSLATION_SOURCES
	translations/growl_notify_en.ts
	translations/growl_notify_pl.ts
)

kadu_module_desc (growl_notify.desc)
qt_wrap_cpp (growl_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations"
)

add_library (growl_notify ${growl_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (growl_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (growl_notify growl_notify-translations)
if (growl_notify STREQUAL STATIC)
	set_target_properties (growl_notify PROPERTIES COMPILE_FLAGS "-DKADULIB")
else (growl_notify STREQUAL STATIC)
	if (APPLE)
		set_target_properties (growl_notify PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -framework Growl -framework CoreFoundation")
	endif (APPLE)
	install (TARGETS growl_notify DESTINATION lib/kadu/modules)
endif (growl_notify STREQUAL STATIC)
