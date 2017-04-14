cmake_minimum_required (VERSION 2.8)

if (ENABLE_DEVELOPER_BUILD AND NOT MSVC AND NOT APPLE)
	set (FAST_LINKING_DEFAULT ON)
else ()
	set (FAST_LINKING_DEFAULT OFF)
endif ()
option (FAST_LINKING "Enable for linking optimized for low RAM (works best with GCC and GNU ld)" "${FAST_LINKING_DEFAULT}")

if (FAST_LINKING)
	message (STATUS "Fast (re)linking enabled")
else ()
	message (STATUS "Fast (re)linking disabled")
endif ()

configure_file (kadu-config.h.in "${CMAKE_CURRENT_BINARY_DIR}/kadu-config.h" ESCAPE_QUOTES)
include_directories ("${CMAKE_CURRENT_BINARY_DIR}")

set (kadu_SRCS
	activate.cpp
	attention-service.cpp
	languages-manager.cpp
	themes.cpp
	translation-loader.cpp
	updates.cpp

	os/single-application/long-lived-lock-file.cpp
	os/single-application/single-application.cpp
)

if (WIN32)
	list (APPEND kadu_SRCS
		kadu.rc
	)
endif ()

if (UNIX AND NOT APPLE)
	list (APPEND kadu_SRCS
		os/x11/x11tools.cpp
	)
endif ()

set (main_SRCS
	main.cpp

	os/single-application/long-lived-lock-file.cpp
	os/single-application/single-application.cpp
)

if (WIN32)
	list (APPEND main_SRCS
		kadu.rc
	)
endif ()

set (kadu_MOC_SRCS
	themes.h
	updates.h
)

set (kadu_SUBDIRS
	accounts
	actions
	avatars
	buddies
	chat
	chat-style
	compression
	configuration
	contacts
	core
	dom
	execution-arguments
	file-system
	file-transfer
	formatted-string
	gui
	html
	icons
	identities
	menu
	message
	misc
	model
	multilogon
	network
	notification
	os
	parser
	plugin
	protocols
	qt
	roster
	services
	ssl
	status
	storage
	talkable
	themes
	url-handlers
	widgets
	windows
)

find_package (LibArchive 2.6.0 REQUIRED)
include (CheckLibraryExists)
check_library_exists ("${LibArchive_LIBRARIES}" "archive_write_set_compression_lzma" "" have_libarchive_lzma_support)
check_library_exists ("${LibArchive_LIBRARIES}" "archive_write_set_compression_xz" "" have_libarchive_xz_support)
if (NOT have_libarchive_lzma_support OR NOT have_libarchive_xz_support)
	message (SEND_ERROR "Your libarchive does not have support for lzma and/or xz archives.")
endif ()

include_directories (${LibArchive_INCLUDE_DIRS})
list (APPEND ADDITIONAL_LIBKADU_LIBRARIES ${LibArchive_LIBRARIES})

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED COMPONENTS X11 Xutil Xfixes)
	if (X11_FOUND AND X11_Xutil_FOUND AND X11_Xfixes_FOUND)
		message (STATUS "Found required X11 libraries: ${X11_X11_LIB};${X11_Xfixes_LIB}")
	else ()
		message (FATAL_ERROR "Could NOT find X11 or X11 Xfixes extension")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	list (APPEND ADDITIONAL_LIBKADU_LIBRARIES ${X11_X11_LIB} ${X11_Xfixes_LIB})
endif ()

if (APPLE)
	list (APPEND ADDITIONAL_LIBKADU_LIBRARIES "-framework carbon")
endif ()

macro (kadu_subdirectory SUBDIR SRCS)
	if (FAST_LINKING)
		add_library (${SUBDIR}
			STATIC
			${SRCS}
		)
		kadu_set_flags (${SUBDIR})
		set_property (TARGET ${SUBDIR} PROPERTY POSITION_INDEPENDENT_CODE TRUE)
		target_compile_definitions (${SUBDIR} PRIVATE libkadu_EXPORTS)
	else ()
		set (tmp "")
		foreach (SRC ${SRCS})
			list (APPEND tmp "${SUBDIR}/${SRC}")
		endforeach ()

		set (${SUBDIR}_SRCS "${tmp}" PARENT_SCOPE)
	endif ()
endmacro ()

foreach (ARG ${kadu_SUBDIRS})
	add_subdirectory (${ARG})

	if (NOT FAST_LINKING)
		list (APPEND kadu_SUBDIRS_SRCS ${${ARG}_SRCS} ${${ARG}_MOC_SRCS})
		include_directories (${${ARG}_INCLUDES})
	endif ()
endforeach ()

add_library (libkadu SHARED ${kadu_SRCS} ${kadu_MOC_SRCS} ${kadu_SUBDIRS_SRCS})
kadu_set_flags (libkadu)
set_property (TARGET libkadu PROPERTY PREFIX "")

if (FAST_LINKING)
	if (APPLE)
		target_link_libraries (libkadu LINK_PRIVATE -Wl,-all_load ${kadu_SUBDIRS})
	else ()
		target_link_libraries (libkadu LINK_PRIVATE -Wl,--start-group -Wl,--whole-archive ${kadu_SUBDIRS} -Wl,--no-whole-archive -Wl,--end-group)
	endif ()
endif ()

qt5_use_modules (libkadu LINK_PRIVATE Core Gui Network Qml Quick QuickWidgets WebKit WebKitWidgets Widgets Xml)
if (UNIX AND NOT APPLE)
	qt5_use_modules (libkadu LINK_PRIVATE DBus X11Extras)
endif ()
if (WIN32)
	qt5_use_modules (libkadu LINK_PRIVATE WinExtras)
endif ()

target_link_libraries (libkadu LINK_PRIVATE ${ADDITIONAL_LIBKADU_LIBRARIES})

add_executable (kadu WIN32 ${main_SRCS})
kadu_set_flags (kadu)
qt5_use_modules (kadu LINK_PRIVATE Core Gui Widgets Network Xml)
target_link_libraries (kadu LINK_PRIVATE libkadu ${ADDITIONAL_LIBRARIES})

if (NOT IS_ABSOLUTE "${KADU_INSTALL_LIB_DIR}")
	set (install_full_lib_dir "${CMAKE_INSTALL_PREFIX}/${KADU_INSTALL_LIB_DIR}")
else ()
	set (install_full_lib_dir "${KADU_INSTALL_LIB_DIR}")
endif ()

set_target_properties (kadu PROPERTIES
	INSTALL_RPATH "${install_full_lib_dir}/kadu"
	BUILD_WITH_INSTALL_RPATH TRUE
)

install (TARGETS libkadu kadu
	EXPORT KaduTargets
	LIBRARY DESTINATION ${KADU_INSTALL_LIB_DIR}/kadu
	RUNTIME DESTINATION ${KADU_INSTALL_BIN_DIR}
)
# TODO this WIN32 check is a workaround for an error on Linux
if (WIN32 AND KADU_INSTALL_SDK)
	install (TARGETS libkadu ARCHIVE DESTINATION ${KADU_INSTALL_SDK_DIR}/lib)
endif ()

if (WIN32)
	add_executable (kadu_c ${main_SRCS})
	kadu_set_flags (kadu_c)
	qt5_use_modules (kadu_c LINK_PRIVATE Core Gui Widgets Network Xml)
	target_link_libraries (kadu_c LINK_PRIVATE libkadu ${ADDITIONAL_LIBRARIES})

	# Do not link against qtmain on Windows
	cmake_policy(SET CMP0020 OLD)

	target_compile_definitions (kadu_c PRIVATE KADU_CONSOLE)

	set_property (TARGET kadu_c PROPERTY Qt5_NO_LINK_QTMAIN TRUE)
	set_property (TARGET kadu PROPERTY Qt5_NO_LINK_QTMAIN TRUE)
	set_property (TARGET libkadu PROPERTY Qt5_NO_LINK_QTMAIN TRUE)

	target_link_libraries (kadu LINK_PRIVATE ws2_32)
	target_link_libraries (libkadu LINK_PRIVATE ws2_32)
	target_link_libraries (kadu_c LINK_PRIVATE ws2_32)

	install (TARGETS kadu_c DESTINATION ${KADU_INSTALL_BIN_DIR})

	file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" "[Paths]\nPlugins = qt-plugins\nTranslations = translations\n")
	install (FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" "kadu.ico" DESTINATION "${KADU_INSTALL_BIN_DIR}")
endif ()

install (TARGETS kadu DESTINATION ${KADU_INSTALL_BIN_DIR})

if (UNIX AND NOT APPLE)
	configure_file (kadu.desktop.in "${CMAKE_CURRENT_BINARY_DIR}/${KADU_DESKTOP_FILE_NAME}")
	install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${KADU_DESKTOP_FILE_NAME}" DESTINATION "${KADU_DESKTOP_FILE_DIR}")

	install (FILES hi16-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps" RENAME kadu.png)
	install (FILES hi22-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/22x22/apps" RENAME kadu.png)
	install (FILES hi24-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps" RENAME kadu.png)
	install (FILES hi32-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps" RENAME kadu.png)
	install (FILES hi48-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps" RENAME kadu.png)
	install (FILES hi64-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps" RENAME kadu.png)
	install (FILES hi128-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps" RENAME kadu.png)
	install (FILES hi256-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps" RENAME kadu.png)
endif ()

install (FILES "chat-style/engine/chat-scripts.js" DESTINATION "${KADU_INSTALL_DATA_DIR}/scripts")

# install api headers
# how to get list: find . -name '*.h' -exec dirname {} +|sort|uniq|sed -e 's|\./||' -e 's|^|\t|'
# TODO: consider using file (GLOB_RECURSE) command here
kadu_api_directories (kadu-core
	.
	accounts
	accounts/filter
	accounts/model
	actions
	actions/chat
	actions/chat-widget
	actions/main
	actions/search
	actions/talkable
	actions/tree-view
	avatars
	buddies
	buddies/model
	chat
	chat/model
	chat-style/engine/chat-engine-adium
	chat-style/engine/chat-engine-kadu
	chat-style/engine
	chat/type
	compression
	configuration
	contacts
	contacts/model
	core
	dom
	execution-arguments
	file-system
	file-transfer
	file-transfer/gui
	formatted-string
	gui
	gui/configuration
	gui/model
	gui/services
	html
	icons
	identities
	identities/model
	menu
	message
	misc
	model
	multilogon
	multilogon/model
	network
	network/proxy
	network/proxy/model
	notification
	notification/listener
	os
	os/generic
	os/qtsingleapplication
	parser
	plugin
	plugin/activation
	plugin/metadata
	plugin/model
	plugin/state
	protocols
	protocols/filter
	protocols/model
	protocols/services
	protocols/services/roster
	provider
	roster
	qt
	services
	ssl
	ssl/gui
	status
	storage
	talkable
	talkable/filter
	talkable/model
	themes
	url-handlers
	widgets
	widgets/configuration
	widgets/dialog
	widgets/chat-widget
	widgets/group-tab-bar
	widgets/webkit-messages-view
	windows
	windows/open-chat-with
)
