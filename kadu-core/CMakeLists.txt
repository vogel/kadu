include (FindPkgConfig)

if (NOT MSVC AND NOT APPLE)
	set (FAST_LINKING_DEFAULT ON)
else (NOT MSVC AND NOT APPLE)
	set (FAST_LINKING_DEFAULT OFF)
endif (NOT MSVC AND NOT APPLE)
option (FAST_LINKING "Enable for linking optimized for low RAM (works best with GCC and GNU ld)" "${FAST_LINKING_DEFAULT}")

if (FAST_LINKING)
	message (STATUS "Fast (re)linking enabled")
else (FAST_LINKING)
	message (STATUS "Fast (re)linking disabled")
endif (FAST_LINKING)

if (NOT WIN32)
	option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)
endif (NOT WIN32)

if (UNIX AND NOT APPLE)
	if (NOT DEFINED WITH_LIBINDICATE_QT OR WITH_LIBINDICATE_QT)
		pkg_search_module (LIBINDICATE_QT indicate-qt)
	endif (NOT DEFINED WITH_LIBINDICATE_QT OR WITH_LIBINDICATE_QT)

	option (WITH_LIBINDICATE_QT "Compile with Messaging Menu (Ayatana) support through libindicate-qt" "${LIBINDICATE_QT_FOUND}")

	if (WITH_LIBINDICATE_QT)
		if (NOT LIBINDICATE_QT_FOUND)
			message (SEND_ERROR "Could NOT find libindicate-qt")
		else (NOT LIBINDICATE_QT_FOUND)
			message (STATUS "Found libindicate-qt. Enabling Ayatana support")
			# We need to link core to libindicate-qt so that indicator_docking
			# doesn't crash at unloading. And it crashes mainly because dbus-glib
			# doesn't support being dlclose()'d, so there is no better workaround.
			link_directories (${LIBINDICATE_QT_LIBRARY_DIRS})
			add_definitions (${LIBINDICATE_QT_CFLAGS_OTHER})
			include_directories (${LIBINDICATE_QT_INCLUDE_DIRS})
			list (APPEND ADDITIONAL_LIBRARIES ${LIBINDICATE_QT_LIBRARIES})
		endif (NOT LIBINDICATE_QT_FOUND)
	else (WITH_LIBINDICATE_QT)
		message (STATUS "Could NOT find libindicate-qt. Disabling Ayatana support")
	endif (WITH_LIBINDICATE_QT)
endif (UNIX AND NOT APPLE)

if (NOT DEFINED NETWORK_IMPLEMENTATION OR "${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
	find_package (QNtrack 010)
endif (NOT DEFINED NETWORK_IMPLEMENTATION OR "${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")

if (NOT DEFINED NETWORK_IMPLEMENTATION)
	if (QNTRACK_FOUND)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: ntrack")
		set (NETWORK_IMPLEMENTATION "ntrack")
	else (QNTRACK_FOUND)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: Qt")
		set (NETWORK_IMPLEMENTATION "Qt")
	endif (QNTRACK_FOUND)
endif ()

set (NETWORK_IMPLEMENTATION "${NETWORK_IMPLEMENTATION}" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)

if ("${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
	list (APPEND ADDITIONAL_LIBRARIES ${QNTRACK_LIBRARIES})
endif ("${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
# The rest of NETWORK_IMPLEMENTATION stuff is handled in network/CMakeLists.txt.

set (kadu_SRCS
	activate.cpp
	debug.cpp
	html_document.cpp
	kadu-application.cpp
	languages-manager.cpp
	themes.cpp
	updates.cpp

	os/qtsingleapplication/qtlocalpeer.cpp
	os/qtsingleapplication/qtlockedfile.cpp
)

if (WIN32)
	list (APPEND kadu_SRCS
		kadu.rc

		os/qtsingleapplication/qtlockedfile_win.cpp
	)
else (WIN32)
	list (APPEND kadu_SRCS
		main.cpp
		main_unix.cpp

		os/qtsingleapplication/qtlockedfile_unix.cpp
	)
endif (WIN32)

if (UNIX AND NOT APPLE)
	list (APPEND kadu_SRCS
		os/x11tools.cpp
	)
endif (UNIX AND NOT APPLE)

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
	emoticons
	file-transfer
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
	else (NOT WIN_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${WIN_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARIES optimized qca2 debug qcad2)

		if (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/lib)
		else (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/bin)
		endif (MSVC)
	endif (NOT WIN_QCA_DIR)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2)
endif (WIN32)

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED COMPONENTS X11 Xutil Xfixes)
	if (X11_FOUND AND X11_Xutil_FOUND AND X11_Xfixes_FOUND)
		message (STATUS "Found required X11 libraries: ${X11_X11_LIB};${X11_Xfixes_LIB}")
	else (X11_FOUND AND X11_Xutil_FOUND AND X11_Xfixes_FOUND)
		message (FATAL_ERROR "Could NOT find X11 or X11 Xfixes extension")
	endif (X11_FOUND AND X11_Xutil_FOUND AND X11_Xfixes_FOUND)

	include_directories (${X11_INCLUDE_DIR})
	list (APPEND ADDITIONAL_LIBRARIES ${X11_X11_LIB} ${X11_Xfixes_LIB})

	if (NOT DESKTOP_ENTRY)
		set (DESKTOP_ENTRY ${CMAKE_CURRENT_BINARY_DIR}/kadu.desktop)
	endif (NOT DESKTOP_ENTRY)
	add_custom_command (OUTPUT ${DESKTOP_ENTRY}
		COMMAND touch ${DESKTOP_ENTRY}
		COMMAND sh ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh ${CMAKE_INSTALL_PREFIX} > ${DESKTOP_ENTRY}
		DEPENDS ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh
		COMMENT "Generating desktop entry file"
	)
	add_custom_target (DESKTOP_ENTRY_FILE ALL
		DEPENDS ${DESKTOP_ENTRY}
	)
endif (UNIX AND NOT APPLE)

if (APPLE)
	list (APPEND ADDITIONAL_LIBRARIES "-framework carbon")
endif (APPLE)

macro (kadu_subdirectory SUBDIR SRCS MOC_SRCS ADDITIONAL_INCLUDES)
	if (FAST_LINKING)
		set (${SUBDIR}_MOC_FILES "")
		if (NOT "${MOC_SRCS}" STREQUAL "")
			qt4_wrap_cpp (${SUBDIR}_MOC_FILES ${MOC_SRCS})
		endif (NOT "${MOC_SRCS}" STREQUAL "")

		add_library (${SUBDIR}
			STATIC
			${SRCS}
			${${SUBDIR}_MOC_FILES}
		)
		set_target_properties (${SUBDIR} PROPERTIES COMPILE_DEFINITIONS KADULIB)
	else (FAST_LINKING)
		set (tmp "")
		foreach (SRC ${SRCS})
			list (APPEND tmp "${SUBDIR}/${SRC}")
		endforeach (SRC)

		set (tmp_moc "")
		foreach (MOC_SRC ${MOC_SRCS})
			list (APPEND tmp_moc "${SUBDIR}/${MOC_SRC}")
		endforeach (MOC_SRC)

		set (${SUBDIR}_SRCS "${tmp}" PARENT_SCOPE)
		set (${SUBDIR}_MOC_SRCS "${tmp_moc}" PARENT_SCOPE)
		set (${SUBDIR}_INCLUDES "${ADDITIONAL_INCLUDES}" PARENT_SCOPE)
	endif (FAST_LINKING)
endmacro (kadu_subdirectory)

macro (kadu_source_subdirectories CURRENT_TARGET)
	foreach (ARG ${ARGN})
		add_subdirectory (${ARG})

		if (FAST_LINKING)
			link_directories (${ARG})
			add_dependencies (${CURRENT_TARGET}
				${ARG}
			)
		else (FAST_LINKING)
			list (APPEND kadu_SUBDIRS_SRCS "${${ARG}_SRCS}")
			qt4_wrap_cpp (MOC_FILES "${${ARG}_MOC_SRCS}")
			include_directories (${${ARG}_INCLUDES})
		endif (FAST_LINKING)
	endforeach (ARG)

	if (FAST_LINKING)
		if (APPLE)
			target_link_libraries (${CURRENT_TARGET} -Wl,-all_load ${ARGN})
		else (APPLE)
			target_link_libraries (${CURRENT_TARGET} -Wl,--start-group -Wl,--whole-archive ${ARGN} -Wl,--no-whole-archive -Wl,--end-group)
		endif (APPLE)
	endif (FAST_LINKING)
endmacro (kadu_source_subdirectories)

if (WIN32)
	set (KADU_MAIN_SOURCES_TARGET kadu_core)
else (WIN32)
	set (KADU_MAIN_SOURCES_TARGET kadu)
endif (WIN32)

if (NOT FAST_LINKING)
	kadu_source_subdirectories (${KADU_MAIN_SOURCES_TARGET} ${kadu_SUBDIRS})
endif (NOT FAST_LINKING)
if (WIN32)
	add_library (${KADU_MAIN_SOURCES_TARGET} SHARED ${kadu_SRCS} ${kadu_SUBDIRS_SRCS} ${MOC_FILES})
else (WIN32)
	add_executable (${KADU_MAIN_SOURCES_TARGET} ${kadu_SRCS} ${kadu_SUBDIRS_SRCS} ${MOC_FILES})
endif (WIN32)
if (FAST_LINKING)
	kadu_source_subdirectories (${KADU_MAIN_SOURCES_TARGET} ${kadu_SUBDIRS})
endif (FAST_LINKING)

set_target_properties (${KADU_MAIN_SOURCES_TARGET} PROPERTIES
	ENABLE_EXPORTS ON
	COMPILE_DEFINITIONS KADULIB
	LINK_INTERFACE_LIBRARIES ""
)

target_link_libraries (${KADU_MAIN_SOURCES_TARGET} ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${ADDITIONAL_LIBRARIES})

if (WIN32)
	add_executable (kadu WIN32 main.cpp main_win32.cpp kadu.rc)
	add_executable (kadu_c main.cpp main_win32.cpp kadu.rc)

	set_target_properties (kadu_c PROPERTIES COMPILE_DEFINITIONS KADU_CONSOLE)

	target_link_libraries (kadu ${KADU_MAIN_SOURCES_TARGET} ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ws2_32)
	target_link_libraries (kadu_c ${KADU_MAIN_SOURCES_TARGET} ${QT_LIBRARIES} ws2_32)

	if (INSTALL_SDK)
		install (TARGETS ${KADU_MAIN_SOURCES_TARGET}
			ARCHIVE DESTINATION ${KADU_SDK_DIR}/lib
			LIBRARY DESTINATION ${KADU_SDK_DIR}/lib
		)
	endif (INSTALL_SDK)

	install (TARGETS ${KADU_MAIN_SOURCES_TARGET} RUNTIME DESTINATION ${KADU_BINDIR})
	install (TARGETS kadu_c DESTINATION ${KADU_BINDIR})

	file (WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\nPlugins = qt-plugins\nTranslations = translations\n")
	install (FILES "${CMAKE_BINARY_DIR}/qt.conf" "kadu.ico" DESTINATION "${KADU_BINDIR}")
endif (WIN32)

install (TARGETS kadu DESTINATION ${KADU_BINDIR})

# install desktop file
if (UNIX AND NOT APPLE)
	install (FILES ${DESKTOP_ENTRY} DESTINATION ${KADU_DESKTOP_FILE_DIR} RENAME ${KADU_DESKTOP_FILE_NAME})

	install (FILES hi16-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps" RENAME kadu.png)
	install (FILES hi22-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/22x22/apps" RENAME kadu.png)
	install (FILES hi24-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps" RENAME kadu.png)
	install (FILES hi32-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps" RENAME kadu.png)
	install (FILES hi48-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps" RENAME kadu.png)
	install (FILES hi64-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps" RENAME kadu.png)
	install (FILES hi128-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps" RENAME kadu.png)
	install (FILES hi256-app-kadu.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps" RENAME kadu.png)
endif (UNIX AND NOT APPLE)

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
	emoticons
	file-transfer
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
	status
	storage
	talkable
	talkable/filter
	talkable/model
	themes
	url-handlers
)
