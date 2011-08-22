set (kadu_SRCS
	activate.cpp
	debug.cpp
	html_document.cpp
	kadu-application.cpp
	languages-manager.cpp
	themes.cpp
	updates.cpp
)

if (UNIX AND NOT APPLE)
	set (kadu_SRCS
		${kadu_SRCS}
		os/x11tools.cpp
	)
endif (UNIX AND NOT APPLE)
if (NOT WIN32)
	set (kadu_SRCS
		${kadu_SRCS}
		os/qtsingleapplication/qtlocalpeer.cpp
		os/qtsingleapplication/qtlockedfile.cpp
	)
endif (NOT WIN32)

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
	themes
	url-handlers
)

qt4_wrap_cpp (MOC_FILES ${kadu_MOC_SRCS})

if (WIN32)
	if (NOT MINGW_QCA_DIR)
		message (SEND_ERROR " Pass path to libqca2 -DMINGW_QCA_DIR=path")
	else (NOT MINGW_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${MINGW_QCA_DIR}/include/QtCrypto)
		set (QCA2_LIBRARY_DIRS ${MINGW_QCA_DIR}/bin)
		set (QCA2_LIBRARIES qca2)
	endif (NOT MINGW_QCA_DIR)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2)
endif (WIN32)

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_FOUND)
		message (STATUS "Found X11 libraries: ${X11_X11_LIB}")
	else (X11_Xfixes_FOUND)
		message (FATAL_ERROR "Could not find X11 libraries")
	endif (X11_FOUND)

	include_directories (${X11_INCLUDE_DIR})

	if (USE_X11_XFIXES)
		if (X11_Xfixes_FOUND)
			message (STATUS "Found X11 Xfixes extension: ${X11_Xfixes_LIB}")
		else (X11_Xfixes_FOUND)
			message (FATAL_ERROR "Could not find X11 Xfixes extension")
		endif (X11_Xfixes_FOUND)
	endif (USE_X11_XFIXES)

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

if (WIN32)
	set (kadu_SRCS
		${kadu_SRCS}
		os/qtsingleapplication/qtlocalpeer.cpp
		os/qtsingleapplication/qtlockedfile.cpp
		os/qtsingleapplication/qtlockedfile_win.cpp
	)

	add_library (kadu_core SHARED ${kadu_SRCS} ${MOC_FILES} kadu_core.rc)
	kadu_source_subdirectories (kadu_core ${kadu_SUBDIRS})

	set_target_properties (kadu_core PROPERTIES
		COMPILE_DEFINITIONS KADULIB
		LINK_INTERFACE_LIBRARIES ""
	)
	target_link_libraries (kadu_core ${QT_LIBRARIES} ${QCA2_LIBRARIES})

	if (MINGW)
		exec_program (windres ${CMAKE_CURRENT_SOURCE_DIR}
			ARGS "kadu_mingw.rc kadu_rc.o"
			OUTPUT_VARIABLE NULL_VAR
		)
		add_executable (kadu WIN32 main.cpp main_win32.cpp kadu_rc.o)
		add_executable (kadu_c main.cpp main_win32.cpp)
	else (MINGW)
		add_executable (kadu WIN32 main.cpp main_win32.cpp kadu.rc)
		add_executable (kadu_c main.cpp main_win32.cpp kadu.rc)
	endif (MINGW)

	set_target_properties (kadu_c PROPERTIES COMPILE_DEFINITIONS KADU_CONSOLE)

	target_link_libraries (kadu kadu_core ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ws2_32)
	target_link_libraries (kadu_c kadu_core ${QT_LIBRARIES} ws2_32)

	install (TARGETS kadu_core
		RUNTIME DESTINATION ${KADU_BIN_DIR}
		ARCHIVE DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib
		LIBRARY DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib
	)
	install (TARGETS kadu_c DESTINATION ${KADU_BIN_DIR})
	install (DIRECTORY "."
		DESTINATION ${KADU_DATA_DIR}/kadu-sdk/include/kadu-core
		FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE
	)
else (WIN32)
	add_executable (kadu main.cpp main_unix.cpp os/qtsingleapplication/qtlockedfile_unix.cpp ${kadu_SRCS} ${MOC_FILES})

	kadu_source_subdirectories (kadu ${kadu_SUBDIRS})

	set_target_properties (kadu PROPERTIES
		ENABLE_EXPORTS ON
		COMPILE_DEFINITIONS KADULIB
		LINK_INTERFACE_LIBRARIES ""
	)

	target_link_libraries (kadu ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${CMAKE_DL_LIBS})

	if (UNIX AND NOT APPLE)
		target_link_libraries (kadu ${X11_X11_LIB})

		if (USE_X11_XFIXES)
			target_link_libraries (kadu ${X11_Xfixes_LIB})
		endif (USE_X11_XFIXES)
	endif (UNIX AND NOT APPLE)
endif (WIN32)

if (${NETWORK_IMPLEMENTATION} STREQUAL "ntrack")
	target_link_libraries (kadu ${QNTRACK_LIBRARIES})
endif (${NETWORK_IMPLEMENTATION} STREQUAL "ntrack")

install (TARGETS kadu DESTINATION ${KADU_BIN_DIR})

# install desktop file
if (UNIX AND NOT APPLE)
	install (FILES ${DESKTOP_ENTRY} DESTINATION ${KADU_DESKTOP_FILE_DIR} RENAME ${KADU_DESKTOP_FILE_NAME})

	# install icons
	install (FILES hi16-app-kadu.png DESTINATION "share/icons/hicolor/16x16/apps" RENAME kadu.png)
	install (FILES hi22-app-kadu.png DESTINATION "share/icons/hicolor/22x22/apps" RENAME kadu.png)
	install (FILES hi24-app-kadu.png DESTINATION "share/icons/hicolor/24x24/apps" RENAME kadu.png)
	install (FILES hi32-app-kadu.png DESTINATION "share/icons/hicolor/32x32/apps" RENAME kadu.png)
	install (FILES hi48-app-kadu.png DESTINATION "share/icons/hicolor/48x48/apps" RENAME kadu.png)
	install (FILES hi64-app-kadu.png DESTINATION "share/icons/hicolor/64x64/apps" RENAME kadu.png)
	install (FILES hi128-app-kadu.png DESTINATION "share/icons/hicolor/128x128/apps" RENAME kadu.png)
	# As of icon-theme-0.5 maximum supported pixmap size is 192, for anything larger one should use SVG.
	#install (FILES hi256-app-kadu.png DESTINATION "share/icons/hicolor/256x256/apps" RENAME kadu.png)
endif (UNIX AND NOT APPLE)

install (FILES "chat/style-engines/chat-scripts.js" DESTINATION ${KADU_DATA_DIR}/scripts)

if (APPLE)
	pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
	include_directories (${SNDFILE_INCLUDE_DIRS})
	link_directories (${SNDFILE_LIBRARY_DIRS})
	target_link_libraries (kadu "-framework carbon")
endif (APPLE)

# install api headers
kadu_api_directories (kadu-core
	accounts/filter
	accounts/model
	accounts
	avatars
	buddies/filter
	buddies/model
	buddies
	chat/filter
	chat/message
	chat/style-engines/style-engine-adium
	chat/style-engines/style-engine-kadu
	chat/style-engines
	chat/type
	chat
	configuration
	contacts/filter
	contacts/model
	contacts
	core
	emoticons
	file-transfer
	gui/actions
	gui/widgets/configuration
	gui/widgets
	gui/windows/open-chat-with
	gui/windows
	gui
	icons
	identities/model
	identities
	misc
	model
	multilogon/model
	multilogon
	network
	notify
	os/generic
	os/qtsingleapplication
	os
	parser
	plugins
	protocols/filter
	protocols/model
	protocols/services
	protocols
	qt
	status
	storage
	themes
	url-handlers
	.
)
