project (windows_integration)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	windows-integration-module.cpp
	windows-integration-plugin-modules-factory.cpp
	windows-integration-plugin-object.cpp
	windows-integration.cpp
	windows-jump-list.cpp
	windows-taskbar-progress.cpp
	windows-thumbnail-toolbar.cpp
)

kadu_plugin (windows_integration
	PLUGIN_SOURCES ${SOURCES}
)

qt5_use_modules (windows_integration LINK_PRIVATE WinExtras)
