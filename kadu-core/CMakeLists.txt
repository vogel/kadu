set (kadu_SRCS
	about.cpp
	action.cpp
	activate.cpp
	chat_edit_box.cpp
	chat_manager.cpp
	chat_message.cpp
	chat_messages_view.cpp
	chat_widget.cpp
	chat_window.cpp
	color_button.cpp
	color_selector.cpp
	config_file.cpp
	configuration_aware_object.cpp
	configuration_window.cpp
	configuration_window_widgets.cpp
	custom_input.cpp
	debug.cpp
	dnshandler.cpp
	emoticons.cpp
	expimp.cpp
	gadu.cpp
	gadu_images_manager.cpp
	gadu-private.cpp
	groups_manager.cpp
	hot_key.cpp
	html_document.cpp
	http_client.cpp
	icons_manager.cpp
	ignore.cpp
	kadu.cpp
	kadu_main_window.cpp
	kadu_parser.cpp
	kadu_text_browser.cpp
	main_configuration_window.cpp
	message.cpp
	message_box.cpp
	misc.cpp
	modules.cpp
	path_list_edit.cpp
	pending_msgs.cpp
	personal_info.cpp
	preview.cpp
	protocol.cpp
	protocols_manager.cpp
	search.cpp
	select_file.cpp
	select_font.cpp
	status_changer.cpp
	status.cpp
	syntax_editor.cpp
	tabbar.cpp
	themes.cpp
	toolbar.cpp
	updates.cpp
	userbox.cpp
	usergroup.cpp
	userinfo.cpp
	userlist.cpp
	userlistelement.cpp
	userlist-private.cpp
)

set (kadu_MOC_SRCS
	about.h
	action.h
	activate.h
	chat_edit_box.h
	chat_manager.h
	chat_messages_view.h
	chat_widget.h
	chat_window.h
	color_button.h
	color_selector.h
	config_file.h
	configuration_aware_object.h
	configuration_window.h
	configuration_window_widgets.h
	custom_input.h
	debug.h
	dnshandler.h
	emoticons.h
	expimp.h
	gadu.h
	gadu_images_manager.h
	gadu-private.h
	groups_manager.h
	hot_key.h
	html_document.h
	http_client.h
	icons_manager.h
	ignore.h
	kadu.h
	kadu_main_window.h
	kadu_parser.h
	kadu_text_browser.h
	main_configuration_window.h
	message.h
	message_box.h
	misc.h
	modules.h
	path_list_edit.h
	pending_msgs.h
	personal_info.h
	preview.h
	protocol.h
	protocols_manager.h
	search.h
	select_file.h
	select_font.h
	status_changer.h
	status.h
	syntax_editor.h
	tabbar.h
	themes.h
	toolbar.h
	updates.h
	userbox.h
	usergroup.h
	userinfo.h
	userlist.h
	userlistelement.h
	userlist-private.h
)

if (UNIX AND NOT APPLE)
	if (NOT DESKTOP_ENTRY)
		set(DESKTOP_ENTRY ${CMAKE_CURRENT_BINARY_DIR}/kadu.desktop)
	endif (NOT DESKTOP_ENTRY)

	add_custom_command(OUTPUT ${DESKTOP_ENTRY}
		COMMAND touch ${DESKTOP_ENTRY}
		COMMAND sh ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh ${CMAKE_INSTALL_PREFIX} >${DESKTOP_ENTRY}
		DEPENDS ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh
		COMMENT "Generating desktop entry file"
	)
	add_custom_target( DESKTOP_ENTRY_FILE ALL
		DEPENDS ${DESKTOP_ENTRY}
	)
endif (UNIX AND NOT APPLE)


qt_wrap_cpp (kadu MOC_FILES ${kadu_MOC_SRCS})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

link_directories (modules)

if (WIN32)
	add_library(kadu_core SHARED ${kadu_SRCS} ${MOC_FILES} kadu_core.rc)
	set_target_properties(kadu_core PROPERTIES DEFINE_SYMBOL KADULIB )
	target_link_libraries (kadu_core ${STATIC_MODULES} ${QT_LIBRARIES} ${LIBGADU_LIBRARIES})

	add_executable(kadu WIN32 main.cpp main_win32.cpp kadu.rc)
	add_executable(kadu_c main.cpp main_win32.cpp kadu.rc)

	set_target_properties(kadu_c PROPERTIES COMPILE_FLAGS "-DKADU_CONSOLE" )

	target_link_libraries (kadu kadu_core ${QT_QTMAIN_LIBRARY})
	target_link_libraries (kadu_c kadu_core)
else (WIN32)
	add_executable (kadu main.cpp main_unix.cpp ${kadu_SRCS} ${MOC_FILES})
	target_link_libraries (kadu ${STATIC_MODULES} ${QT_LIBRARIES} ${LIBGADU_LIBRARIES})
endif (WIN32)


install (TARGETS kadu DESTINATION bin)

# install desktop file
if (UNIX AND NOT APPLE)
	install(FILES ${DESKTOP_ENTRY} DESTINATION share/applications)
endif (UNIX AND NOT APPLE)


# install icons
install (FILES hi16-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-16.png)
install (FILES hi22-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-22.png)
install (FILES hi24-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-24.png)
install (FILES hi32-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu.png)
install (FILES hi48-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-48.png)
install (FILES hi64-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-64.png)
install (FILES hi128-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-128.png)
install (FILES hi256-app-kadu.png DESTINATION "share/pixmaps" RENAME kadu-256.png)

