if (NOT MSVC)
	option (FAST_LINKING "Enable for faster linking, optimized for low RAM (works best with GCC and GNU ld)" ON)
else (NOT MSVC)
	option (FAST_LINKING "Enable for faster linking, optimized for low RAM (works best with GCC and GNU ld)" OFF)
endif (NOT MSVC)

if (FAST_LINKING)
	message (STATUS "Fast linking enabled")
endif (FAST_LINKING)

if (UNIX)
	option (SIG_HANDLING_ENABLED "Define if you want system signals handling" ON)

	if (NOT APPLE)
		option (USE_X11_XFIXES "Use X11 XFixes extension (Desktop Linux)" ON)

		pkg_search_module (INDICATEQT indicate-qt)
		if (INDICATEQT_FOUND)
			message (STATUS "libindicate-qt devel found. Enabling Ayatana support")
			option (WITH_LIBINDICATE_QT "Compile with Messaging Menu (Ayatana) support trough libindicate-qt" ON)
		else (INDICATEQT_FOUND)
			message (STATUS "libindicate-qt devel NOT FOUND. Disabling Ayatana support")
			option (WITH_LIBINDICATE_QT "Compile with Messaging Menu (Ayatana) support trough libindicate-qt" OFF)
		endif (INDICATEQT_FOUND)
	endif (NOT APPLE)
endif (UNIX)

if (NOT NETWORK_IMPLEMENTATION)
	include (FindQNtrack)
	if (QNTRACK_FOUND)
		set (NETWORK_IMPLEMENTATION "ntrack" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: ntrack")
	else (QNTRACK_FOUND)
		set (NETWORK_IMPLEMENTATION "Qt" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: Qt")
	endif (QNTRACK_FOUND)
elseif (${NETWORK_IMPLEMENTATION} STREQUAL "ntrack")
	set (QNtrack_FIND_REQUIRED true)
	include (FindQNtrack)
endif (NOT NETWORK_IMPLEMENTATION)

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
	list (APPEND kadu_SRCS
		os/x11tools.cpp
	)
endif (UNIX AND NOT APPLE)
if (NOT WIN32)
	list (APPEND kadu_SRCS
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

set (ADDITIONAL_LIBRARIES "")

if (WIN32)
	if (NOT WIN_QCA_DIR)
		message (SEND_ERROR "Pass path to libqca2 -DWIN_QCA_DIR=path")
	else (NOT WIN_QCA_DIR)
		set (QCA2_INCLUDE_DIRS ${WIN_QCA_DIR}/include/QtCrypto)

		if (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/lib)
		else (MSVC)
			set (QCA2_LIBRARY_DIRS ${WIN_QCA_DIR}/bin)
		endif (MSVC)

		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set (QCA2_LIBRARIES qcad2)
		else (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set (QCA2_LIBRARIES qca2)
		endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	endif (NOT WIN_QCA_DIR)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2)
endif (WIN32)

link_directories (${QCA2_LIBRARY_DIRS})
add_definitions (${QCA2_CFLAGS_OTHER})
include_directories (${QCA2_INCLUDE_DIRS})

if (UNIX AND NOT APPLE)
	if (WITH_LIBINDICATE_QT)
		# We need to link core to libindicate-qt so that indicator_docking
		# doesn't crash at unloading. And it crashes mainly because dbus-glib
		# doesn't support being dlclose()'d, so there is no better workaround.
		link_directories (${INDICATEQT_LIBRARY_DIRS})
		add_definitions (${INDICATEQT_CFLAGS_OTHER})
		include_directories (${INDICATEQT_INCLUDE_DIRS})
		list (APPEND ADDITIONAL_LIBRARIES ${INDICATEQT_LIBRARIES})
	endif (WITH_LIBINDICATE_QT)

	if (${NETWORK_IMPLEMENTATION} STREQUAL "ntrack")
		list (APPEND ADDITIONAL_LIBRARIES ${QNTRACK_LIBRARIES})
	endif (${NETWORK_IMPLEMENTATION} STREQUAL "ntrack")

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
			set (tmp "${tmp};${SUBDIR}/${SRC}")
		endforeach (SRC)

		set (tmp_moc "")
		foreach (MOC_SRC ${MOC_SRCS})
			set (tmp_moc "${tmp_moc};${SUBDIR}/${MOC_SRC}")
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
			list (APPEND kadu_SRCS "${${ARG}_SRCS}")
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
	list (APPEND kadu_SRCS
		os/qtsingleapplication/qtlocalpeer.cpp
		os/qtsingleapplication/qtlockedfile.cpp
		os/qtsingleapplication/qtlockedfile_win.cpp
	)

	if (NOT FAST_LINKING)
		kadu_source_subdirectories (kadu_core ${kadu_SUBDIRS})
	endif (NOT FAST_LINKING)
	add_library (kadu_core SHARED ${kadu_SRCS} ${MOC_FILES} kadu_core.rc)
	if (FAST_LINKING)
		kadu_source_subdirectories (kadu_core ${kadu_SUBDIRS})
	endif (FAST_LINKING)

	set_target_properties (kadu_core PROPERTIES
		COMPILE_DEFINITIONS KADULIB
		LINK_INTERFACE_LIBRARIES ""
	)
	target_link_libraries (kadu_core ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${ADDITIONAL_LIBRARIES})

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

	if (INSTALL_SDK)
		install (TARGETS kadu_core
			ARCHIVE DESTINATION ${KADU_SDK_DIR}/lib
			LIBRARY DESTINATION ${KADU_SDK_DIR}/lib
		)
	endif (INSTALL_SDK)

	install (TARGETS kadu_core RUNTIME DESTINATION ${KADU_BIN_DIR})
	install (TARGETS kadu_c DESTINATION ${KADU_BIN_DIR})
else (WIN32)
	if (NOT FAST_LINKING)
		kadu_source_subdirectories (kadu ${kadu_SUBDIRS})
	endif (NOT FAST_LINKING)
	add_executable (kadu main.cpp main_unix.cpp os/qtsingleapplication/qtlockedfile_unix.cpp ${kadu_SRCS} ${MOC_FILES})
	if (FAST_LINKING)
		kadu_source_subdirectories (kadu ${kadu_SUBDIRS})
	endif (FAST_LINKING)

	set_target_properties (kadu PROPERTIES
		ENABLE_EXPORTS ON
		COMPILE_DEFINITIONS KADULIB
		LINK_INTERFACE_LIBRARIES ""
	)

	target_link_libraries (kadu ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${CMAKE_DL_LIBS} ${ADDITIONAL_LIBRARIES})

	if (UNIX AND NOT APPLE)
		target_link_libraries (kadu ${X11_X11_LIB})

		if (USE_X11_XFIXES)
			target_link_libraries (kadu ${X11_Xfixes_LIB})
		endif (USE_X11_XFIXES)
	endif (UNIX AND NOT APPLE)
endif (WIN32)

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
	chat/model
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
	gui/actions/chat
	gui/actions
	gui/widgets/configuration
	gui/widgets
	gui/windows/open-chat-with
	gui/windows
	gui
	icons
	identities/model
	identities
	message
	misc
	model
	multilogon/model
	multilogon
	network/proxy/model
	network/proxy
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
	talkable/filter
	talkable/model
	talkable
	themes
	url-handlers
	.
)
