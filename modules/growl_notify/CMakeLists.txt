set (SOURCES
	growl_notify.cpp
	growlnotifier.cpp
)

set (MOC_SOURCES
	growl_notify.h
	growlnotifier.h
)

kadu_module_desc (growl_notify.desc)
qt_wrap_cpp (growl_notify MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (growl_notify ${growl_notify} ${SOURCES} ${MOC_FILES})
if (growl_notify STREQUAL STATIC)
	set_target_properties(growl_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (growl_notify STREQUAL STATIC)
	if (APPLE)
		set_target_properties(growl_notify PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -framework Growl -framework CoreFoundation")
	endif (APPLE)
	install (TARGETS growl_notify DESTINATION lib/kadu/modules)
endif (growl_notify STREQUAL STATIC)

