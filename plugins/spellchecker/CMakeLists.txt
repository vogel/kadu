project (spellchecker)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/spellchecker-configuration.cpp
	highlighter.cpp
	spellchecker.cpp
	spellchecker-plugin.cpp
	suggester.cpp
)

set (MOC_SOURCES
	highlighter.h
	spellchecker.h
	spellchecker-plugin.h
	suggester.h
)

set (TRANSLATION_SOURCES
	translations/spellchecker_cs.ts
	translations/spellchecker_de.ts
	translations/spellchecker_en.ts
	translations/spellchecker_pl.ts
)

set (CONFIGURATION_FILES
	configuration/spellchecker.ui
)

if (WIN32)
	set (USE_ASPELL TRUE)

	if (NOT MINGW_ASPELL_DIR)
		message (SEND_ERROR " Pass path to aspell -DMINGW_ASPELL_DIR=path")
	else (NOT MINGW_ASPELL_DIR)
		set (ASPELL_FOUND true)
		set (ASPELL_INCLUDE_DIR ${MINGW_ASPELL_DIR}/aspell-dev-0-50-3-3/include)
		set (ASPELL_LIBRARY_DIR ${MINGW_ASPELL_DIR}/bin)
		set (ASPELL_LIBRARIES aspell-15)
	endif (NOT MINGW_ASPELL_DIR)
else (WIN32)
if (APPLE)
	LIST(APPEND SOURCES macspellchecker.mm)
	LIST(APPEND MOC_SOURCES macspellchecker.h)
else (APPLE)
	if (WITH_ENCHANT AND NOT WITH_ASPELL)
		set (USE_ENCHANT TRUE)
		set (FORCE_ENGINE TRUE)
	else (WITH_ENCHANT AND NOT WITH_ASPELL)
		set (USE_ASPELL TRUE)
		if (WITH_ASPELL)
			set (FORCE_ENGINE TRUE)
		endif (WITH_ASPELL)
	
		if (WITH_ENCHANT)
			message (STATUS "Cannot use both enchant and aspell. Using aspell")
		endif (WITH_ENCHANT)
	endif (WITH_ENCHANT AND NOT WITH_ASPELL)
	
	if (USE_ASPELL)
		include (FindASPELL)
	
		if (NOT ASPELL_FOUND)
			if (FORCE_ENGINE)
				message (FATAL_ERROR "aspell not found!")
			else (FORCE_ENGINE)
				message (STATUS "aspell not found. Falling back to enchant")
				set (USE_ASPELL FALSE)
				set (USE_ENCHANT TRUE)
			endif (FORCE_ENGINE)
		endif (NOT ASPELL_FOUND)
	endif (USE_ASPELL)
	
	if (USE_ENCHANT)
		include (FindPkgConfig)
		pkg_search_module (ENCHANT enchant)
	
		if (NOT ENCHANT_FOUND)
			message (FATAL_ERROR "enchant not found!")
		endif (NOT ENCHANT_FOUND)
	endif (USE_ENCHANT)
endif (APPLE)
endif (WIN32)

if (USE_ASPELL)
	message (STATUS "Using aspell as backend. To force enchant use -DWITH_ENCHANT=ON")
	add_definitions (-DHAVE_ASPELL)

	set (LIBRARIES ${ASPELL_LIBRARIES})
	link_directories (${ASPELL_LIBRARY_DIR})
	include_directories (${ASPELL_INCLUDE_DIR})
else (USE_ASPELL)
if (NOT APPLE)
	message (STATUS "Using enchant as backend")
	add_definitions (-DHAVE_ENCHANT)

	set (LIBRARIES ${ENCHANT_LIBRARIES})
	link_directories (${ENCHANT_LIBRARY_DIRS})
	include_directories (${ENCHANT_INCLUDE_DIRS})
	add_definitions (${ENCHANT_DEFINITIONS})
endif (NOT APPLE)
endif (USE_ASPELL)

kadu_plugin (spellchecker
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
