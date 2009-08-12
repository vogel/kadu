set (kadu_SRCS

	accounts/account.cpp
	accounts/account-manager.cpp
	accounts/accounts-aware-object.cpp

	accounts/model/accounts-model.cpp

	chat/chat.cpp
	chat/chat-manager.cpp
	chat/conference-chat.cpp
	chat/simple-chat.cpp

	chat/chat_message.cpp
	chat/chat_styles_manager.cpp

	chat/message/formatted-message.cpp
	chat/message/formatted-message-part.cpp
	chat/message/message.cpp
	chat/message/pending-messages-manager.cpp

	chat/style-engines/chat_engine_adium.cpp
	chat/style-engines/chat_engine_kadu.cpp

	contacts/contact.cpp
	contacts/contact-account-data.cpp
	contacts/contact-data.cpp
	contacts/contact-kadu-data.cpp
	contacts/contact-list.cpp
	contacts/contact-list-configuration-helper.cpp
	contacts/contact-list-mime-data-helper.cpp
	contacts/contact-manager.cpp
	contacts/contact-remove-predicate-object.cpp
	contacts/contact-set.cpp
	contacts/contact-set-configuration-helper.cpp
	contacts/contacts-aware-object.cpp
	contacts/group.cpp
	contacts/group-manager.cpp
	contacts/ignored-helper.cpp

	contacts/model/abstract-contacts-model.cpp
	contacts/model/contact-list-model.cpp
	contacts/model/contacts-model.cpp
	contacts/model/contacts-model-base.cpp
	contacts/model/contacts-model-proxy.cpp
	
	contacts/model/filter/account-contact-filter.cpp
	contacts/model/filter/anonymous-contact-filter.cpp
	contacts/model/filter/anonymous-without-messages-contact-filter.cpp
	contacts/model/filter/group-contact-filter.cpp
	contacts/model/filter/has-description-contact-filter.cpp
	contacts/model/filter/offline-contact-filter.cpp
	contacts/model/filter/online-and-description-contact-filter.cpp
	contacts/model/filter/online-contact-filter.cpp

	configuration/account-data-manager.cpp
	configuration/config-file-data-manager.cpp
	configuration/configuration-aware-object.cpp
	configuration/configuration-file.cpp
	configuration/configuration-manager.cpp
	configuration/contact-account-data-manager.cpp
	configuration/notifier-configuration-data-manager.cpp
	configuration/storable-object.cpp
	configuration/storable-string-list.cpp
	configuration/uuid-storable-object.cpp
	configuration/xml-configuration-file.cpp

	core/core.cpp
	core/core-actions.cpp

	file-transfer/file-transfer.cpp
	file-transfer/file-transfer-manager.cpp

	gui/hot-key.cpp

	gui/actions/action.cpp
	gui/actions/actions.cpp
	gui/actions/action-description.cpp

	gui/widgets/account-contacts-list-widget.cpp
	gui/widgets/chat-edit-box.cpp
	gui/widgets/chat_messages_view.cpp
	gui/widgets/chat-widget.cpp
	gui/widgets/chat-widget-actions.cpp
	gui/widgets/chat-widget-manager.cpp
	gui/widgets/color-button.cpp
	gui/widgets/color-selector.cpp
	gui/widgets/contact-account-data-widget.cpp
	gui/widgets/contact-info-panel.cpp
	gui/widgets/contacts-list-widget.cpp
	gui/widgets/contacts-list-widget-delegate.cpp
	gui/widgets/contacts-list-widget-menu-manager.cpp
	gui/widgets/custom_input.cpp
	gui/widgets/group-tab-bar.cpp
	gui/widgets/image-widget.cpp
	gui/widgets/kadu_text_browser.cpp
	gui/widgets/path-list-edit.cpp
	gui/widgets/proxy-group-box.cpp
	gui/widgets/preview.cpp
	gui/widgets/select-file.cpp
	gui/widgets/select-font.cpp
	gui/widgets/status-button.cpp
	gui/widgets/status-buttons.cpp
	gui/widgets/status-menu.cpp
	gui/widgets/syntax-editor.cpp
	gui/widgets/toolbar.cpp
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
	gui/widgets/configuration/notifier-configuration-widget.cpp
	gui/widgets/configuration/notify-group-box.cpp
	gui/widgets/configuration/notify-tree-widget.cpp

	gui/windows/about.cpp
	gui/windows/add-buddy-window.cpp
	gui/windows/chat_window.cpp
	gui/windows/choose-description.cpp
	gui/windows/configuration-window.cpp
	gui/windows/contact-data-manager.cpp
	gui/windows/contact-data-window.cpp
	gui/windows/group-properties-window.cpp
	gui/windows/image-dialog.cpp
	gui/windows/kadu-window.cpp
	gui/windows/kadu-window-actions.cpp
	gui/windows/main-configuration-window.cpp
	gui/windows/main-window.cpp
	gui/windows/message-box.cpp
	gui/windows/open-chat-with/open-chat-with.cpp
	gui/windows/open-chat-with/open-chat-with-contact-list-runner.cpp
	gui/windows/open-chat-with/open-chat-with-runner-manager.cpp
	gui/windows/password-window.cpp
	gui/windows/syntax-editor-window.cpp
	gui/windows/window-notifier-window.cpp
	gui/windows/your-accounts.cpp

	misc/coding-conversion.cpp
	misc/date-time.cpp
	misc/misc.cpp
	misc/path-conversion.cpp
	misc/syntax-list.cpp

	model/first-empty.cpp

	modules/module-data.cpp

	notify/account-notification.cpp
	notify/contact-notify-data.cpp
	notify/new_message_notification.cpp
	notify/notification.cpp
	notify/notification-manager.cpp
	notify/notifier.cpp
	notify/notify-configuration-ui-handler.cpp
	notify/notify-event.cpp
	notify/status_changed_notification.cpp
	notify/window-notifier.cpp

	parser/parser.cpp

	protocols/connection-error-notification.cpp
	protocols/protocol.cpp
	protocols/protocol-menu-manager.cpp
	protocols/protocols-manager.cpp
	protocols/model/protocols-model.cpp
	protocols/services/chat-service.cpp

	status/description-manager.cpp
	status/description-model.cpp
	status/status.cpp
	status/status-container-aware-object.cpp
	status/status-container-manager.cpp
	status/status-group.cpp
	status/status-group-manager.cpp
	status/status-type.cpp
	status/status-type-manager.cpp

	activate.cpp
	debug.cpp
	dnshandler.cpp
	emoticons.cpp
	expimp.cpp
	html_document.cpp
	http_client.cpp
	icons-manager.cpp
	ignore.cpp
	modules.cpp
	status_changer.cpp
	themes.cpp
	updates.cpp
)

if (UNIX AND NOT APPLE)
	set (kadu_SRCS
		${kadu_SRCS}
		x11tools.cpp
	)
endif (UNIX AND NOT APPLE)

set (kadu_MOC_SRCS

	accounts/account.h
	accounts/account-manager.h

	accounts/model/accounts-model.h

	chat/chat.h
	chat/chat-manager.h
	#chat/conference-chat.h
	#chat/simple-chat.h

	chat/message/message.h
	chat/message/pending-messages-manager.h

	chat/chat_styles_manager.h
	chat/style-engines/chat_engine_kadu.h

	contacts/contact.h
	contacts/contact-account-data.h
	contacts/contact-data.h
	#contacts/contact-list.h
	#contacts/contact-list-configuration-helper.h
	contacts/contact-manager.h
	#contacts/contact-set.h
	contacts/group.h
	contacts/group-manager.h

	#contacts/model/abstract-contacts-model.h
	contacts/model/contact-list-model.h
	contacts/model/contacts-model.h
	contacts/model/contacts-model-base.h
	contacts/model/contacts-model-proxy.h
	
	contacts/model/filter/abstract-contact-filter.h
	contacts/model/filter/account-contact-filter.h
	contacts/model/filter/anonymous-contact-filter.h
	contacts/model/filter/anonymous-without-messages-contact-filter.h
	contacts/model/filter/group-contact-filter.h
	contacts/model/filter/has-description-contact-filter.h
	contacts/model/filter/offline-contact-filter.h
	contacts/model/filter/online-and-description-contact-filter.h
	contacts/model/filter/online-contact-filter.h

	configuration/configuration-window-data-manager.h
	configuration/notifier-configuration-data-manager.h

	core/core.h

	file-transfer/file-transfer.h
	file-transfer/file-transfer-manager.h

	gui/actions/action.h
	gui/actions/actions.h
	gui/actions/action-description.h

	gui/widgets/account-contacts-list-widget.h
	gui/widgets/account-create-widget.h
	gui/widgets/account-edit-widget.h
	gui/widgets/chat-edit-box.h
	gui/widgets/chat_messages_view.h
	gui/widgets/chat-widget.h
	gui/widgets/chat-widget-actions.h
	gui/widgets/chat-widget-manager.h
	gui/widgets/color-button.h
	gui/widgets/color-selector.h
	gui/widgets/contact-account-data-widget.h
	gui/widgets/contact-info-panel.h
	gui/widgets/contacts-list-widget.h
	gui/widgets/contacts-list-widget-delegate.h
	gui/widgets/custom_input.h
	gui/widgets/group-tab-bar.h
	gui/widgets/kadu_text_browser.h
	gui/widgets/path-list-edit.h
	gui/widgets/preview.h
	gui/widgets/select-file.h
	gui/widgets/select-font.h
	gui/widgets/status-button.h
	gui/widgets/status-buttons.h
	gui/widgets/status-menu.h
	gui/widgets/syntax-editor.h
	gui/widgets/toolbar.h

	#gui/widgets/configuration/config-action-button.h
	gui/widgets/configuration/config-check-box.h
	#gui/widgets/configuration/config-widget-value.h
	gui/widgets/configuration/configuration-widget.h
	#gui/widgets/configuration/kadu-scroll-area.h
	gui/widgets/configuration/config-section.h
	#gui/widgets/configuration/config-tab.h
	gui/widgets/configuration/notifier-configuration-widget.h
	gui/widgets/configuration/notify-group-box.h

	gui/windows/about.h
	gui/windows/add-buddy-window.h
	gui/windows/chat_window.h
	gui/windows/choose-description.h
	gui/windows/configuration-window.h
	gui/windows/contact-data-manager.h
	gui/windows/contact-data-window.h
	gui/windows/group-properties-window.h
	gui/windows/kadu-window.h
	gui/windows/kadu-window-actions.h
	gui/windows/main-configuration-window.h
	gui/windows/main-window.h
	gui/windows/message-box.h
	gui/windows/open-chat-with/open-chat-with.h
	gui/windows/password-window.h
	gui/windows/syntax-editor-window.h
	gui/windows/your-accounts.h

	model/first-empty.h

	misc/syntax-list.h

	notify/account-notification.h
	notify/new_message_notification.h
	notify/notification.h
	notify/notification-manager.h
	notify/notifier.h
	notify/notify-configuration-ui-handler.h
	notify/status_changed_notification.h
	notify/window-notifier.h

	protocols/connection-error-notification.h
	protocols/protocol.h
	protocols/protocols-manager.h
	protocols/model/protocols-model.h
	protocols/services/chat-image-service.h
	protocols/services/chat-service.h
	protocols/services/contact-list-service.h
	protocols/services/file-transfer-service.h
	protocols/services/personal-info-service.h
	protocols/services/search-service.h

	status/description-manager.h
	status/description-model.h
	status/status-container.h
	status/status-container-manager.h

	dnshandler.h
	emoticons.h
	expimp.h
	http_client.h
	icons-manager.h
	ignore.h
	modules.h
	status_changer.h
	themes.h
	updates.h
)

#if (UNIX AND NOT APPLE)
#	set (kadu_MOC_SRCS
#		${kadu_MOC_SRCS}
#		x11tools.h
#	)
#endif (UNIX AND NOT APPLE)

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
	target_link_libraries (kadu_core ${QT_LIBRARIES} ${LIBGADU_LIBRARIES} ${STATIC_MODULES})
	set_property(TARGET kadu_core PROPERTY LINK_INTERFACE_LIBRARIES "")

	add_executable(kadu WIN32 main.cpp main_win32.cpp kadu.rc)
	add_executable(kadu_c main.cpp main_win32.cpp kadu.rc)

	set_target_properties(kadu_c PROPERTIES COMPILE_FLAGS "-DKADU_CONSOLE" )

	target_link_libraries (kadu kadu_core ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES})
	target_link_libraries (kadu_c kadu_core ${QT_LIBRARIES})

	install (TARGETS kadu_core RUNTIME DESTINATION ${KADU_BIN_DIR} ARCHIVE DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib LIBRARY DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib)
	install (TARGETS kadu_c DESTINATION ${KADU_BIN_DIR})
	install (DIRECTORY "." DESTINATION ${KADU_DATA_DIR}/kadu-sdk/include/kadu-core FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE )
else (WIN32)	
	add_executable (kadu main.cpp main_unix.cpp ${kadu_SRCS} ${MOC_FILES})
	set_target_properties(kadu PROPERTIES ENABLE_EXPORTS ON)
	set_target_properties(kadu PROPERTIES COMPILE_FLAGS "-DKADULIB" )

	target_link_libraries (kadu ${STATIC_MODULES} ${QT_LIBRARIES} ${LIBGADU_LIBRARIES})
endif (WIN32)


install (TARGETS kadu DESTINATION ${KADU_BIN_DIR})

# install desktop file
if (UNIX AND NOT APPLE)
	install(FILES ${DESKTOP_ENTRY} DESTINATION share/applications)

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
endif(UNIX AND NOT APPLE)

install (FILES "chat/style-engines/adium_style_scripts.js" DESTINATION ${KADU_DATA_DIR}/scripts)

if (APPLE)
	pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
	include_directories (${SNDFILE_INCLUDE_DIRS})
	link_directories (${SNDFILE_LIBRARY_DIRS})
endif (APPLE)
