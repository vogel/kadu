set (kadu_SRCS

	accounts/account.cpp
	accounts/account_data.cpp
	accounts/account_manager.cpp
	accounts/accounts_aware_object.cpp

	accounts/model/accounts-model.cpp

	chat/chat_manager.cpp
	chat/chat_message.cpp
	chat/chat_styles_manager.cpp
	chat/style-engines/chat_engine_kadu.cpp

	contacts/contact.cpp
	contacts/contact-account-data.cpp
	contacts/contact-data.cpp
	contacts/contact-kadu-data.cpp
	contacts/contact-list.cpp
	contacts/contact-list-configuration-helper.cpp
	contacts/contact-list-mime-data-helper.cpp
	contacts/contact-manager.cpp
	contacts/contact-module-data.cpp
	contacts/contacts-aware-object.cpp
	contacts/group.cpp
	contacts/group-manager.cpp
	contacts/ignored-helper.cpp

	contacts/model/abstract-contacts-model.cpp
	contacts/model/contact-list-model.cpp
	contacts/model/contacts-model.cpp
	contacts/model/contacts-model-proxy.cpp
	
	contacts/model/filter/anonymous-contact-filter.cpp
	contacts/model/filter/anonymous-without-messages-contact-filter.cpp
	contacts/model/filter/group-contact-filter.cpp
	contacts/model/filter/has-description-contact-filter.cpp
	contacts/model/filter/offline-contact-filter.cpp
	contacts/model/filter/online-and-description-contact-filter.cpp
	contacts/model/filter/online-contact-filter.cpp

	configuration/account-data-manager.cpp
	configuration/config-file-data-manager.cpp
	configuration/contact-account-data-manager.cpp
	configuration/storable-object.cpp

	configuration/widgets/manage-accounts.cpp

	gui/widgets/contact-account-data-widget.cpp

	gui/widgets/chat_edit_box.cpp
	gui/widgets/chat_messages_view.cpp
	gui/widgets/chat_widget.cpp
	gui/widgets/contacts-list-widget.cpp
	gui/widgets/contacts-list-widget-delegate.cpp
	gui/widgets/contacts-list-widget-menu-manager.cpp
	gui/widgets/custom_input.cpp
	gui/widgets/group-tab-bar.cpp
	gui/widgets/kadu_text_browser.cpp
	gui/widgets/preview.cpp
	gui/widgets/tool-tip-class-manager.cpp

	gui/widgets/configuration/config-action-button.cpp
	gui/widgets/configuration/config-check-box.cpp
	gui/widgets/configuration/config-color-button.cpp
	gui/widgets/configuration/config-combo-box.cpp
	gui/widgets/configuration/config-gg-password-edit.cpp
	gui/widgets/configuration/config-hot-key-edit.cpp
	gui/widgets/configuration/config-label.cpp
	gui/widgets/configuration/config-line-edit.cpp
	gui/widgets/configuration/config-list-widget.cpp
	gui/widgets/configuration/config-manage-accounts.cpp
	gui/widgets/configuration/config-path-list-edit.cpp
	gui/widgets/configuration/config-preview.cpp
	gui/widgets/configuration/config-select-file.cpp
	gui/widgets/configuration/config-select-font.cpp
	gui/widgets/configuration/config-slider.cpp
	gui/widgets/configuration/config-spin-box.cpp
	gui/widgets/configuration/config-syntax-editor.cpp
	gui/widgets/configuration/config-widget-value.cpp
	gui/widgets/configuration/config-widget.cpp
	gui/widgets/configuration/configuration-widget.cpp
	gui/widgets/configuration/config-section.cpp
	gui/widgets/configuration/config-group-box.cpp
	gui/widgets/configuration/config-tab.cpp

	gui/windows/chat_window.cpp
	gui/windows/configuration-window.cpp
	gui/windows/contact-data-window.cpp

	protocols/protocol.cpp
	protocols/protocol-menu-manager.cpp
	protocols/protocols_manager.cpp
	protocols/status.cpp

	protocols/services/chat-service.cpp

	about.cpp
	action.cpp
	activate.cpp
	color_button.cpp
	color_selector.cpp
	config_file.cpp
	configuration_aware_object.cpp
	debug.cpp
	dnshandler.cpp
	emoticons.cpp
	expimp.cpp
	hot_key.cpp
	html_document.cpp
	http_client.cpp
	icons_manager.cpp
	ignore.cpp
	kadu.cpp
	kadu_main_window.cpp
	kadu_parser.cpp
	main_configuration_window.cpp
	message.cpp
	message_box.cpp
	misc.cpp
	modules.cpp
	path_list_edit.cpp
	pending_msgs.cpp
	personal_info.cpp
	search.cpp
	select_file.cpp
	select_font.cpp
	status_changer.cpp
	syntax_editor.cpp
	themes.cpp
	toolbar.cpp
	updates.cpp
	xml_config_file.cpp
)

if (UNIX AND NOT APPLE)
	set (kadu_SRCS
		${kadu_SRCS}
		x11tools.cpp
	)
endif (UNIX AND NOT APPLE)

set (kadu_MOC_SRCS

	accounts/account.h
	accounts/account_data.h
	accounts/account_manager.h

	accounts/model/accounts-model.h

	chat/chat_manager.h

	contacts/contact.h
	contacts/contact-account-data.h
	contacts/contact-data.h
	contacts/contact-list.h
	contacts/contact-manager.h
	contacts/contact-module-data.h
	contacts/contact-list-configuration-helper.h
	contacts/group-manager.h

	contacts/model/abstract-contacts-model.h
	contacts/model/contact-list-model.h
	contacts/model/contacts-model.h
	contacts/model/contacts-model-proxy.h
	
	contacts/model/filter/abstract-contact-filter.h
	contacts/model/filter/anonymous-contact-filter.h
	contacts/model/filter/anonymous-without-messages-contact-filter.h
	contacts/model/filter/group-contact-filter.h
	contacts/model/filter/has-description-contact-filter.h
	contacts/model/filter/offline-contact-filter.h
	contacts/model/filter/online-and-description-contact-filter.h
	contacts/model/filter/online-contact-filter.h

	configuration/config-file-data-manager.h
	configuration/configuration-window-data-manager.h

	configuration/widgets/manage-accounts.h

	gui/widgets/chat_edit_box.h
	gui/widgets/chat_messages_view.h
	gui/widgets/chat_widget.h
	gui/widgets/contact-account-data-widget.h
	gui/widgets/contacts-list-widget.h
	gui/widgets/contacts-list-widget-delegate.h
	gui/widgets/custom_input.h
	gui/widgets/group-tab-bar.h
	gui/widgets/kadu_text_browser.h
	gui/widgets/preview.h

	gui/widgets/configuration/config-action-button.h
	gui/widgets/configuration/config-check-box.h
	gui/widgets/configuration/config-widget-value.h
	gui/widgets/configuration/config-widget.h
	gui/widgets/configuration/configuration-widget.h
	gui/widgets/configuration/kadu-scroll-area.h
	gui/widgets/configuration/config-section.h
	gui/widgets/configuration/config-tab.h

	gui/windows/chat_window.h
	gui/windows/configuration-window.h
	gui/windows/contact-data-window.h

	protocols/protocol.h
	protocols/protocol_factory.h
	protocols/protocols_manager.h

	protocols/services/chat-service.h
	protocols/services/contact-list-service.h

	about.h
	action.h
	activate.h
	color_button.h
	color_selector.h
	config_file.h
	configuration_aware_object.h
	debug.h
	dnshandler.h
	emoticons.h
	expimp.h
	hot_key.h
	html_document.h
	http_client.h
	icons_manager.h
	ignore.h
	kadu.h
	kadu_main_window.h
	kadu_parser.h
	main_configuration_window.h
	message.h
	message_box.h
	misc.h
	modules.h
	path_list_edit.h
	pending_msgs.h
	personal_info.h
	search.h
	select_file.h
	select_font.h
	status_changer.h
	syntax_editor.h
	themes.h
	toolbar.h
	updates.h
)

if (UNIX AND NOT APPLE)
	set (kadu_MOC_SRCS
		${kadu_MOC_SRCS}
		x11tools.h
	)
endif (UNIX AND NOT APPLE)

if (UNIX AND NOT APPLE)
	if (NOT DESKTOP_ENTRY)
		set (DESKTOP_ENTRY ${CMAKE_CURRENT_BINARY_DIR}/kadu.desktop)
	endif (NOT DESKTOP_ENTRY)

	add_custom_command(OUTPUT ${DESKTOP_ENTRY}
		COMMAND touch ${DESKTOP_ENTRY}
		COMMAND sh ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh ${CMAKE_INSTALL_PREFIX} >${DESKTOP_ENTRY}
		DEPENDS ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh
		COMMENT "Generating desktop entry file"
	)
	add_custom_target(DESKTOP_ENTRY_FILE ALL
		DEPENDS ${DESKTOP_ENTRY}
	)
endif (UNIX AND NOT APPLE)

qt_wrap_cpp (kadu MOC_FILES ${kadu_MOC_SRCS})

if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
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
	set_target_properties(kadu PROPERTIES COMPILE_FLAGS "-DKADULIB" )

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

if (APPLE)
	pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
	include_directories (${SNDFILE_INCLUDE_DIRS})
	link_directories (${SNDFILE_LIBRARY_DIRS})
endif (APPLE)
