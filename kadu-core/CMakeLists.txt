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
	main.cpp
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

qt_wrap_cpp (kadu MOC_FILES ${kadu_MOC_SRCS})

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_executable (kadu ${kadu_SRCS} ${MOC_FILES})

link_directories (modules)
target_link_libraries (kadu ${QT_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QT3SUPPORT_LIBRARY} ${STATIC_MODULES} ${LIBGADU_LIBRARIES})

install (TARGETS kadu DESTINATION bin)
