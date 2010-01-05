set (SOURCES
	kadudbus.cpp
	kadurootobject.cpp
)

set (MOC_SOURCES
	kadudbus.h
	kadurootobject.h
)

kadu_module_desc(dbus.desc)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

qt_wrap_cpp (dbus MOC_FILES ${MOC_SOURCES})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (dbus ${dbus} ${SOURCES} ${MOC_FILES})
#add_custom_target (dbus-translations DEPENDS ${TRANSLATION_FILES})
#add_dependencies (dbus dbus-translations)

if (dbus STREQUAL STATIC)
	set_target_properties(dbus PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (dbus STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(dbus kadu_core)
	endif (WIN32)
	install (TARGETS dbus DESTINATION ${MODULEDIR})
endif (dbus STREQUAL STATIC)
