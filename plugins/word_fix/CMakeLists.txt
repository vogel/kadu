project (word_fix)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	word-fix.cpp
	word-fix-formatted-string-visitor.cpp
)

set (MOC_SOURCES
	word-fix.h
)

set (CONFIGURATION_FILES
	configuration/word_fix.ui
)

set (DATA_FILES
	data/wf_default_list.data
)

kadu_plugin (word_fix
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
