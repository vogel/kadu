set (SOURCES
	spellchecker.cpp
	highlighter.cpp
)

set (MOC_SOURCES
	spellchecker.h
	highlighter.h
)

set (TRANSLATION_SOURCES
	translations/spellchecker_en.ts
	translations/spellchecker_de.ts
	translations/spellchecker_fr.ts
	translations/spellchecker_it.ts
	translations/spellchecker_pl.ts
)

set (CONFIGURATION_FILES
	configuration/spellchecker.ui
)

if (WITH_ENCHANT AND NOT WITH_ASPELL)
	set (USE_ENCHANT TRUE)
	set (FORCE_ENGINE TRUE)
else (WITH_ENCHANT AND NOT WITH_ASPELL)
	set (USE_ASPELL TRUE)
	if (WITH_ASPELL OR NOT WITH_ENCHANT)
		set (FORCE_ENGINE TRUE)
	endif (WITH_ASPELL OR NOT WITH_ENCHANT)

	if (WITH_ENCHANT)
		message (STATUS "Cannot use both enchant and aspell. Using enchant")
	endif (WITH_ENCHANT)
endif (WITH_ENCHANT AND NOT WITH_ASPELL)

if (USE_ASPELL)
	include (FindASPELL)

	if (NOT ASPELL_FOUND)
		if (FORCE_ENGINE)
			message (FATAL_ERROR "aspell not found!")
		else (FORCE_ENGINE)
			message (STATUS "aspell not found. Falling back to enchant")
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

if (USE_ASPELL)
	message (STATUS "Using aspell as backend. To force enchant use -DWITH_ENCHANT=ON")

	set (LIBRARIES ${ASPELL_LIBRARIES})
	include_directories (${ASPELL_INCLUDE_DIR})
	add_definitions (${ASPELL_DEFINITIONS})
	add_definitions (-DHAVE_ASPELL)
else (USE_ASPELL)
	message (STATUS "Using enchant as backend")

	set (LIBRARIES ${ENCHANT_LIBRARIES})
	include_directories (${ENCHANT_INCLUDE_DIRS})
	add_definitions (${ENCHANT_DEFINITIONS})
	add_definitions (-DHAVE_ENCHANT)
endif (USE_ASPELL)

kadu_module (spellchecker
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${LIBRARIES}
)
