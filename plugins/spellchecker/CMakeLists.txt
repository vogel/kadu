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

set (CONFIGURATION_FILES
	configuration/spellchecker.ui
)

if (APPLE)
	list (APPEND SOURCES macspellchecker.mm)
	list (APPEND MOC_SOURCES macspellchecker.h)
else (APPLE)
	if (WIN32)
		set (USE_ASPELL TRUE)

		if (NOT WIN_ASPELL_DIR)
			message (SEND_ERROR "Pass path to aspell -DWIN_ASPELL_DIR=path")
		else (NOT WIN_ASPELL_DIR)
			set (ASPELL_FOUND TRUE)
			set (ASPELL_LIBRARIES aspell)
			set (ASPELL_INCLUDE_DIR ${WIN_ASPELL_DIR}/include)

			if (MSVC)
				set (ASPELL_LIBRARY_DIR ${WIN_ASPELL_DIR}/lib)
			else (MSVC)
				set (ASPELL_LIBRARY_DIR ${WIN_ASPELL_DIR}/bin)
			endif (MSVC)
		endif (NOT WIN_ASPELL_DIR)
	else (WIN32)
		if (WITH_ASPELL AND NOT WITH_ENCHANT)
			set (USE_ASPELL TRUE)
			set (FORCE_ENGINE TRUE)
		else (WITH_ASPELL AND NOT WITH_ENCHANT)
			set (USE_ENCHANT TRUE)
			if (WITH_ENCHANT)
				set (FORCE_ENGINE TRUE)
			endif (WITH_ENCHANT)
		
			if (WITH_ASPELL)
				message (STATUS "Cannot use both enchant and aspell. Using enchant")
			endif (WITH_ASPELL)
		endif (WITH_ASPELL AND NOT WITH_ENCHANT)
		
		if (USE_ENCHANT)
			include (FindPkgConfig)
			pkg_search_module (ENCHANT enchant)
		
			if (NOT ENCHANT_FOUND)
				if (FORCE_ENGINE)
					message (SEND_ERROR "enchant not found!")
				else (FORCE_ENGINE)
					message (STATUS "enchant not found. Falling back to aspell")
					set (USE_ENCHANT FALSE)
					set (USE_ASPELL TRUE)
				endif (FORCE_ENGINE)
			endif (NOT ENCHANT_FOUND)
		endif (USE_ENCHANT)
		
		if (USE_ASPELL)
			include (FindASPELL)
		
			if (NOT ASPELL_FOUND)
				message (SEND_ERROR "aspell not found!")
			endif (NOT ASPELL_FOUND)
		endif (USE_ASPELL)
	endif (WIN32)
	
	if (USE_ENCHANT)
		message (STATUS "Using enchant as backend. To force aspell use -DWITH_ASPELL=ON")
		add_definitions (-DHAVE_ENCHANT)
	
		set (LIBRARIES ${ENCHANT_LIBRARIES})
		link_directories (${ENCHANT_LIBRARY_DIRS})
		include_directories (${ENCHANT_INCLUDE_DIRS})
		add_definitions (${ENCHANT_DEFINITIONS})
	else (USE_ENCHANT)
		message (STATUS "Using aspell as backend")
		add_definitions (-DHAVE_ASPELL)
	
		set (LIBRARIES ${ASPELL_LIBRARIES})
		link_directories (${ASPELL_LIBRARY_DIR})
		include_directories (${ASPELL_INCLUDE_DIR})
	endif (USE_ENCHANT)
endif (APPLE)

kadu_plugin (spellchecker
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
