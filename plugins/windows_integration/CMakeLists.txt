project (windows_integration)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	jump-list.cpp
	windows-integration-module.cpp
	windows-integration-plugin-modules-factory.cpp
	windows-integration-plugin-object.cpp
	windows-integration.cpp
	windows-jump-list-service.cpp
	windows-taskbar-progress.cpp
	windows-thumbnail-toolbar.cpp
)

if (WIN32)
	list (APPEND SOURCES
		windows-jump-list.cpp)
else (WIN32)
	list (APPEND SOURCES
		console-jump-list.cpp)
endif (WIN32)

kadu_plugin (windows_integration
	PLUGIN_SOURCES ${SOURCES}
)

kadu_add_plugin_test (windows_integration windows-jump-list-service)

if (WIN32)
	qt5_use_modules (windows_integration LINK_PRIVATE WinExtras)
endif (WIN32)
