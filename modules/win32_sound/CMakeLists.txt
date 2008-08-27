set (SOURCES
	win32_sound.cpp
)

set (MOC_SOURCES
	win32_sound.h
)

set (TRANSLATION_SOURCES
	#	translations/win32_sound_de.ts
	#	translations/win32_sound_fr.ts
	#	translations/win32_sound_it.ts
	#	translations/win32_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/win32_sound.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (win32_sound.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (win32_sound MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_library (win32_sound ${win32_sound} ${SOURCES} ${MOC_FILES})
add_custom_target (win32_sound-translations DEPENDS ${TRANSLATION_FILES})
if (win32_sound STREQUAL STATIC)
set_target_properties(tabs PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (win32_sound STREQUAL STATIC)
if (WIN32)
target_link_libraries(win32_sound kadu_core sound)
endif (WIN32)
endif (win32_sound STREQUAL STATIC)
add_dependencies (win32_sound win32_sound-translations)

install (TARGETS win32_sound DESTINATION lib/kadu/modules)
