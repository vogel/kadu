include (FindPkgConfig)

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

if (NOT WIN32)
	option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)
endif ()

if (UNIX AND NOT APPLE)
	if (NOT DEFINED WITH_LIBINDICATE_QT OR WITH_LIBINDICATE_QT)
		pkg_search_module (LIBINDICATE_QT indicate-qt)
	endif ()

	option (WITH_LIBINDICATE_QT "Compile with Messaging Menu (Ayatana) support through libindicate-qt" "${LIBINDICATE_QT_FOUND}")

	if (WITH_LIBINDICATE_QT)
		if (NOT LIBINDICATE_QT_FOUND)
			message (SEND_ERROR "Could NOT find libindicate-qt")
		else ()
			message (STATUS "Found libindicate-qt. Enabling Ayatana support")
			# We need to link core to libindicate-qt so that indicator_docking
			# doesn't crash at unloading. And it crashes mainly because dbus-glib
			# doesn't support being dlclose()'d, so there is no better workaround.
			link_directories (${LIBINDICATE_QT_LIBRARY_DIRS})
			add_definitions (${LIBINDICATE_QT_CFLAGS_OTHER})
			include_directories (${LIBINDICATE_QT_INCLUDE_DIRS})
			list (APPEND ADDITIONAL_LIBRARIES ${LIBINDICATE_QT_LIBRARIES})
		endif ()
	else ()
		message (STATUS "Could NOT find libindicate-qt. Disabling Ayatana support")
	endif ()
endif ()

if (NOT DEFINED NETWORK_IMPLEMENTATION OR "${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
	find_package (QNtrack 010)
endif ()

if (NOT DEFINED NETWORK_IMPLEMENTATION)
	if (QNTRACK_FOUND)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: ntrack")
		set (NETWORK_IMPLEMENTATION "ntrack")
	else ()
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: Qt")
		set (NETWORK_IMPLEMENTATION "Qt")
	endif ()
endif ()

set (NETWORK_IMPLEMENTATION "${NETWORK_IMPLEMENTATION}" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)

if ("${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
	list (APPEND ADDITIONAL_LIBRARIES ${QNTRACK_LIBRARIES})
endif ()
# The rest of NETWORK_IMPLEMENTATION stuff is handled in network/CMakeLists.txt.

set (kadu_SRCS
	activate.cpp
	debug.cpp
	kadu-application.cpp
	languages-manager.cpp
	themes.cpp
	updates.cpp

	os/qtsingleapplication/qtlocalpeer.cpp
	os/qtsingleapplication/qtlockedfile.cpp
)

if (WIN32)
	list (APPEND kadu_SRCS
		os/qtsingleapplication/qtlockedfile_win.cpp
		kadu.rc
	)
else ()
	list (APPEND kadu_SRCS
		os/qtsingleapplication/qtlockedfile_unix.cpp
	)
endif ()

if (UNIX AND NOT APPLE)
	list (APPEND kadu_SRCS
		os/x11tools.cpp
	)
endif ()

set (main_SRCS
	main.cpp
)

if (WIN32)
	list (APPEND main_SRCS
		main_win32.cpp
		kadu.rc
	)
else ()
	list (APPEND main_SRCS
		main_unix.cpp
	)
endif ()

set (kadu_MOC_SRCS
	kadu-application.h
	themes.h
	updates.h
)

set (kadu_SUBDIRS
	accounts
	avatars
	buddies
	chat
	configuration
	contacts
	core
	dom
	file-transfer
	formatted-string
	gui
	icons
	identities
	message
	misc
	model
	multilogon
	network
	notify
	os
	parser
	plugins
	protocols
	qt
	services
	status
	storage
	talkable
	themes
	url-handlers
)

qt4_wrap_cpp (MOC_FILES ${kadu_MOC_SRCS})

if (WIN32)
	if (NOT WIN_QCA_DIR)
		message (SEND_ERROR "Pass path to libqca2 -DWIN_QCA_DIR=path")
	else ()
		set (QCA2_INCLUDE_DIRS ${WIN_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARIES optimized qca2 debug qcad2)

		if (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/lib)
		else ()
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/bin)
		endif ()
	endif ()
else ()
	pkg_search_module (QCA2 REQUIRED qca2)
endif ()

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED COMPONENTS X11 Xutil Xfixes)
	if (X11_FOUND AND X11_Xutil_FOUND AND X11_Xfixes_FOUND)
		message (STATUS "Found required X11 libraries: ${X11_X11_LIB};${X11_Xfixes_LIB}")
	else ()
		message (FATAL_ERROR "Could NOT find X11 or X11 Xfixes extension")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	list (APPEND ADDITIONAL_LIBRARIES ${X11_X11_LIB} ${X11_Xfixes_LIB})
endif ()

if (APPLE)
	list (APPEND ADDITIONAL_LIBRARIES "-framework carbon")
endif ()

macro (kadu_subdirectory SUBDIR SRCS MOC_SRCS ADDITIONAL_INCLUDES)
	if (FAST_LINKING)
		set (${SUBDIR}_MOC_FILES "")
		if (NOT "${MOC_SRCS}" STREQUAL "")
			qt4_wrap_cpp (${SUBDIR}_MOC_FILES ${MOC_SRCS})
		endif ()

		add_library (${SUBDIR}
			STATIC
			${SRCS}
			${${SUBDIR}_MOC_FILES}
		)
		set_target_properties (${SUBDIR} PROPERTIES COMPILE_DEFINITIONS KADULIB)
		if (NOT WIN32)
			set_target_properties (${SUBDIR} PROPERTIES COMPILE_FLAGS -fPIC)
		endif ()
	else ()
		set (tmp "")
		foreach (SRC ${SRCS})
			list (APPEND tmp "${SUBDIR}/${SRC}")
		endforeach ()

		set (tmp_moc "")
		foreach (MOC_SRC ${MOC_SRCS})
			list (APPEND tmp_moc "${SUBDIR}/${MOC_SRC}")
		endforeach ()

		set (${SUBDIR}_SRCS "${tmp}" PARENT_SCOPE)
		set (${SUBDIR}_MOC_SRCS "${tmp_moc}" PARENT_SCOPE)
		set (${SUBDIR}_INCLUDES "${ADDITIONAL_INCLUDES}" PARENT_SCOPE)
	endif ()
endmacro ()

macro (kadu_source_subdirectories CURRENT_TARGET)
	foreach (ARG ${ARGN})
		add_subdirectory (${ARG})

		if (FAST_LINKING)
			link_directories (${ARG})
			add_dependencies (${CURRENT_TARGET}
				${ARG}
			)
		else ()
			list (APPEND kadu_SUBDIRS_SRCS "${${ARG}_SRCS}")
			qt4_wrap_cpp (MOC_FILES "${${ARG}_MOC_SRCS}")
			include_directories (${${ARG}_INCLUDES})
		endif ()
	endforeach ()

	if (FAST_LINKING)
		if (APPLE)
			target_link_libraries (${CURRENT_TARGET} -Wl,-all_load ${ARGN})
		else ()
			target_link_libraries (${CURRENT_TARGET} -Wl,--start-group -Wl,--whole-archive ${ARGN} -Wl,--no-whole-archive -Wl,--end-group)
		endif ()
	endif ()
endmacro ()

if (NOT FAST_LINKING)
	kadu_source_subdirectories (libkadu ${kadu_SUBDIRS})
endif ()
add_library (libkadu SHARED ${kadu_SRCS} ${kadu_SUBDIRS_SRCS} ${MOC_FILES})
if (FAST_LINKING)
	kadu_source_subdirectories (libkadu ${kadu_SUBDIRS})
endif ()
target_link_libraries (libkadu ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${ADDITIONAL_LIBRARIES})
set_target_properties (libkadu PROPERTIES
	COMPILE_DEFINITIONS KADULIB
	PREFIX ""
)

add_executable (kadu WIN32 ${main_SRCS})
target_link_libraries (kadu libkadu ${QT_QTMAIN_LIBRARY})
set_target_properties (kadu PROPERTIES
	INSTALL_RPATH "${KADU_LIBDIR}"
	BUILD_WITH_INSTALL_RPATH TRUE
)

install (TARGETS libkadu kadu
	LIBRARY DESTINATION ${KADU_LIBDIR}
	RUNTIME DESTINATION ${KADU_BINDIR}
)
# TODO this WIN32 check is a workaround for an error on Linux
if (WIN32 AND INSTALL_SDK)
	install (TARGETS libkadu ARCHIVE DESTINATION ${KADU_SDK_DIR}/lib)
endif ()

if (WIN32)
	add_executable (kadu_c ${main_SRCS})

	set_target_properties (kadu_c PROPERTIES COMPILE_DEFINITIONS KADU_CONSOLE)

	target_link_libraries (kadu ws2_32)
	target_link_libraries (kadu_c libkadu ws2_32)

	install (TARGETS kadu_c DESTINATION ${KADU_BINDIR})

	file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" "[Paths]\nPlugins = qt-plugins\nTranslations = translations\n")
	install (FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" "kadu.ico" DESTINATION "${KADU_BINDIR}")
endif ()

install (TARGETS kadu DESTINATION ${KADU_BINDIR})

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

install (FILES "chat/style-engines/chat-scripts.js" DESTINATION "${KADU_DATADIR}/scripts")

# install api headers
# how to get list: find . -name '*.h' -exec dirname {} +|sort|uniq|sed -e 's|\./||' -e 's|^|\t|'
# TODO: consider using file (GLOB_RECURSE) command here
kadu_api_directories (kadu-core
	.
	accounts
	accounts/filter
	accounts/model
	avatars
	buddies
	buddies/model
	chat
	chat/model
	chat/style-engines
	chat/style-engines/chat-engine-adium
	chat/style-engines/chat-engine-kadu
	chat/type
	configuration
	contacts
	contacts/model
	core
	dom
	emoticons
	file-transfer
	formatted-string
	gui
	gui/actions
	gui/actions/chat
	gui/model
	gui/widgets
	gui/widgets/configuration
	gui/windows
	gui/windows/open-chat-with
	icons
	identities
	identities/model
	message
	misc
	model
	multilogon
	multilogon/model
	network
	network/proxy
	network/proxy/model
	notify
	notify/listener
	notify/notification
	os
	os/generic
	os/qtsingleapplication
	parser
	plugins
	plugins/model
	protocols
	protocols/filter
	protocols/model
	protocols/services
	protocols/services/roster
	provider
	qt
	services
	status
	storage
	talkable
	talkable/filter
	talkable/model
	themes
	url-handlers
)
