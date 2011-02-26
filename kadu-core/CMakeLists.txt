set (kadu_SRCS
	accounts/account.cpp
	accounts/account-details.cpp
	accounts/account-manager.cpp
	accounts/account-proxy-settings.cpp
	accounts/account-shared.cpp
	accounts/accounts-aware-object.cpp

	accounts/filter/have-multilogon-filter.cpp
	accounts/filter/id-validity-filter.cpp
	accounts/filter/protocol-filter.cpp
	accounts/filter/writeable-contacts-list-filter.cpp

	accounts/model/accounts-model.cpp
	accounts/model/accounts-proxy-model.cpp

	avatars/avatar.cpp
	avatars/avatar-job-manager.cpp
	avatars/avatar-job-runner.cpp
	avatars/avatar-manager.cpp
	avatars/avatar-shared.cpp

	buddies/buddies-aware-object.cpp
	buddies/buddy.cpp
	buddies/buddy-additional-data-delete-handler.cpp
	buddies/buddy-additional-data-delete-handler-manager.cpp
	buddies/buddy-kadu-data.cpp
	buddies/buddy-list.cpp
	buddies/buddy-list-mime-data-helper.cpp
	buddies/buddy-manager.cpp
	buddies/buddy-or-contact.cpp
	buddies/buddy-preferred-manager.cpp
	buddies/buddy-remove-predicate-object.cpp
	buddies/buddy-search-criteria.cpp
	buddies/buddy-set.cpp
	buddies/buddy-shared.cpp
	buddies/group.cpp
	buddies/group-manager.cpp
	buddies/group-shared.cpp

	buddies/filter/account-buddy-filter.cpp
	buddies/filter/anonymous-buddy-filter.cpp
	buddies/filter/anonymous-without-messages-buddy-filter.cpp
	buddies/filter/blocked-buddy-filter.cpp
	buddies/filter/buddy-name-filter.cpp
	buddies/filter/group-buddy-filter.cpp
	buddies/filter/has-description-buddy-filter.cpp
	buddies/filter/mobile-buddy-filter.cpp
	buddies/filter/non-buddy-filter.cpp
	buddies/filter/offline-buddy-filter.cpp
	buddies/filter/online-and-description-buddy-filter.cpp
	buddies/filter/pending-messages-filter.cpp

	buddies/model/abstract-buddies-model.cpp
	buddies/model/buddy-data-extractor.cpp
	buddies/model/buddy-list-model.cpp
	buddies/model/buddies-model.cpp
	buddies/model/buddies-model-base.cpp
	buddies/model/buddies-model-proxy.cpp
	buddies/model/groups-model.cpp

	chat/aggregate-chat-manager.cpp
	chat/chat.cpp
	chat/chat-details-aggregate.cpp
	chat/chat-details-conference.cpp
	chat/chat-details-simple.cpp
	chat/chat-geometry-data.cpp
	chat/chat-manager.cpp
	chat/chat-shared.cpp
	chat/chat-styles-manager.cpp
	chat/html-messages-renderer.cpp
	chat/recent-chat-manager.cpp

	chat/filter/chat-name-filter.cpp

	chat/message/formatted-message.cpp
	chat/message/formatted-message-part.cpp
	chat/message/message.cpp
	chat/message/message-render-info.cpp
	chat/message/pending-messages-manager.cpp
	chat/message/message-shared.cpp

	chat/style-engines/chat-engine-adium/adium-style.cpp
	chat/style-engines/chat-engine-adium/adium-time-formatter.cpp
	chat/style-engines/chat-engine-adium/chat-engine-adium.cpp
	chat/style-engines/chat-engine-kadu/chat-engine-kadu.cpp

	chat/type/chat-type.cpp
	chat/type/chat-type-aware-object.cpp
	chat/type/chat-type-conference.cpp
	chat/type/chat-type-manager.cpp
	chat/type/chat-type-simple.cpp

	configuration/config-file-data-manager.cpp
	configuration/configuration-aware-object.cpp
	configuration/configuration-file.cpp
	configuration/configuration-manager.cpp
	configuration/main-configuration.cpp
	configuration/notifier-configuration-data-manager.cpp
	configuration/toolbar-configuration-manager.cpp
	configuration/xml-configuration-file.cpp

	contacts/contact.cpp
	contacts/contact-details.cpp
	contacts/contact-manager.cpp
	contacts/contact-parser-tags.cpp
	contacts/contact-set.cpp
	contacts/contact-set-configuration-helper.cpp
	contacts/contact-shared.cpp

	contacts/filter/contact-no-unloaded-account-filter.cpp

	contacts/model/buddy-contact-model.cpp
	contacts/model/contact-data-extractor.cpp
	contacts/model/contact-list-model.cpp

	core/core.cpp
	core/core-actions.cpp
	core/crash-aware-object.cpp

	emoticons/emoticon-selector-button.cpp
	emoticons/emoticon-selector.cpp
	emoticons/emoticons-manager.cpp
	emoticons/emots-walker.cpp
	emoticons/prefix-node.cpp

	gui/hot-key.cpp

	gui/actions/action.cpp
	gui/actions/actions.cpp
	gui/actions/action-description.cpp

	gui/widgets/account-buddy-list-widget.cpp
	gui/widgets/account-avatar-widget.cpp
	gui/widgets/accounts-combo-box.cpp
	gui/widgets/buddies-list-view.cpp
	gui/widgets/buddies-list-view-avatar-painter.cpp
	gui/widgets/buddies-list-view-delegate.cpp
	gui/widgets/buddies-list-view-delegate-configuration.cpp
	gui/widgets/buddies-list-view-item-painter.cpp
	gui/widgets/buddies-list-view-menu-item.cpp
	gui/widgets/buddies-list-view-menu-manager.cpp
	gui/widgets/buddies-list-widget.cpp
	gui/widgets/buddy-avatar-widget.cpp
	gui/widgets/buddy-contacts-table-delegate.cpp
	gui/widgets/buddy-contacts-table-item.cpp
	gui/widgets/buddy-contacts-table-model.cpp
	gui/widgets/buddy-contacts-table-model-proxy.cpp
	gui/widgets/buddy-contacts-table.cpp
	gui/widgets/buddy-general-configuration-widget.cpp
	gui/widgets/buddy-groups-configuration-widget.cpp
	gui/widgets/buddy-personal-info-configuration-widget.cpp
	gui/widgets/buddy-options-configuration-widget.cpp
	gui/widgets/buddy-info-panel.cpp
	gui/widgets/chat-edit-box.cpp
	gui/widgets/chat-edit-box-size-manager.cpp
	gui/widgets/chat-messages-view.cpp
	gui/widgets/chat-view-network-access-manager.cpp
	gui/widgets/chat-widget.cpp
	gui/widgets/chat-widget-actions.cpp
	gui/widgets/chat-widget-manager.cpp
	gui/widgets/choose-identity-widget.cpp
	gui/widgets/color-button.cpp
	gui/widgets/color-selector.cpp
	gui/widgets/custom-input.cpp
	gui/widgets/delayed-line-edit.cpp
	gui/widgets/filter-widget.cpp
	gui/widgets/group-tab-bar.cpp
	gui/widgets/groups-combo-box.cpp
	gui/widgets/identities-combo-box.cpp
	gui/widgets/kadu-web-view.cpp
	gui/widgets/line-edit-clear-button.cpp
	gui/widgets/line-edit-with-clear-button.cpp
	gui/widgets/modal-configuration-widget.cpp
	gui/widgets/path-edit.cpp
	gui/widgets/path-list-edit.cpp
	gui/widgets/progress-icon.cpp
	gui/widgets/progress-label.cpp
	gui/widgets/protocols-combo-box.cpp
	gui/widgets/proxy-group-box.cpp
	gui/widgets/preview.cpp
	gui/widgets/select-buddy-combo-box.cpp
	gui/widgets/select-buddy-popup.cpp
	gui/widgets/select-file.cpp
	gui/widgets/select-font.cpp
	gui/widgets/status-button.cpp
	gui/widgets/status-buttons.cpp
	gui/widgets/status-menu.cpp
	gui/widgets/syntax-editor.cpp
	gui/widgets/toolbar.cpp
	gui/widgets/tool-tip-class-manager.cpp

	gui/widgets/configuration/buddy-list-background-colors-widget.cpp
	gui/widgets/configuration/config-action-button.cpp
	gui/widgets/configuration/config-check-box.cpp
	gui/widgets/configuration/config-color-button.cpp
	gui/widgets/configuration/config-combo-box.cpp
	gui/widgets/configuration/config-gg-password-edit.cpp
	gui/widgets/configuration/config-hot-key-edit.cpp
	gui/widgets/configuration/config-label.cpp
	gui/widgets/configuration/config-line-separator.cpp
	gui/widgets/configuration/config-line-edit.cpp
	gui/widgets/configuration/config-list-widget.cpp
	gui/widgets/configuration/config-path-list-edit.cpp
	gui/widgets/configuration/config-preview.cpp
	gui/widgets/configuration/config-radio-button.cpp
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
	gui/windows/buddy-data-window.cpp
	gui/windows/buddy-data-window-aware-object.cpp
	gui/windows/buddy-delete-window.cpp
	gui/windows/chat-window.cpp
	gui/windows/choose-description.cpp
	gui/windows/configuration-window.cpp
	gui/windows/group-properties-window.cpp
	gui/windows/kadu-window.cpp
	gui/windows/kadu-window-actions.cpp
	gui/windows/main-configuration-window.cpp
	gui/windows/main-window.cpp
	gui/windows/merge-buddies-window.cpp
	gui/windows/message-dialog.cpp
	gui/windows/modules-window.cpp
	gui/windows/multilogon-window.cpp
	gui/windows/password-window.cpp
	gui/windows/progress-window.cpp
	gui/windows/search-window.cpp
	gui/windows/search-window-actions.cpp
	gui/windows/subscription-window.cpp
	gui/windows/syntax-editor-window.cpp
	gui/windows/updates-dialog.cpp
	gui/windows/window-notifier-window.cpp
	gui/windows/your-accounts.cpp

	gui/windows/open-chat-with/open-chat-with.cpp
	gui/windows/open-chat-with/open-chat-with-contact-list-runner.cpp
	gui/windows/open-chat-with/open-chat-with-runner-manager.cpp

	identities/model/identity-model.cpp
	identities/identities-aware-object.cpp
	identities/identity.cpp
	identities/identity-manager.cpp
	identities/identity-shared.cpp

	misc/coding-conversion.cpp
	misc/date-time.cpp
	misc/misc.cpp
	misc/path-conversion.cpp
	misc/syntax-list.cpp

	model/actions-proxy-model.cpp

	multilogon/model/multilogon-model.cpp

	multilogon/multilogon-session.cpp

	notify/account-notification.cpp
	notify/buddy-notify-data.cpp
	notify/chat-notification.cpp
	notify/new-message-notification.cpp
	notify/multilogon-notification.cpp
	notify/notification.cpp
	notify/notification-manager.cpp
	notify/notifier.cpp
	notify/notify-configuration-ui-handler.cpp
	notify/notify-event.cpp
	notify/status-changed-notification.cpp
	notify/window-notifier.cpp

	os/generic/compositing-aware-object.cpp
	os/generic/system-info.cpp
	os/generic/url-opener.cpp

	parser/parser.cpp

	protocols/connection-error-notification.cpp
	protocols/protocol.cpp
	protocols/protocols-aware-object.cpp
	protocols/protocols-manager.cpp
	protocols/filter/can-register-protocol-filter.cpp
	protocols/model/protocols-model.cpp
	protocols/model/protocols-model-proxy.cpp
	protocols/services/chat-service.cpp
	protocols/services/chat-image-service.cpp
	protocols/services/contact-list-service.cpp

	qt/long-validator.cpp

	status/base-status-container.cpp
	status/description-manager.cpp
	status/description-model.cpp
	status/status.cpp
	status/status-actions.cpp
	status/status-changer.cpp
	status/status-changer-manager.cpp
	status/status-container-aware-object.cpp
	status/status-container-manager.cpp
	status/status-group.cpp
	status/status-group-manager.cpp
	status/status-type.cpp
	status/status-type-manager.cpp

	storage/named-storable-object.cpp
	storage/module-data.cpp
	storage/shared.cpp
	storage/shared-base.cpp
	storage/storable-object.cpp
	storage/storable-string-list.cpp
	storage/storage-point.cpp
	storage/uuid-storable-object.cpp

	themes/emoticon-theme-manager.cpp
	themes/icon-theme-manager.cpp
	themes/theme.cpp
	themes/theme-manager.cpp

	url-handlers/mail-url-handler.cpp
	url-handlers/standard-url-handler.cpp
	url-handlers/url-handler-manager.cpp

	activate.cpp
	debug.cpp
	dnshandler.cpp
	html_document.cpp
	icons-manager.cpp
	kadu-application.cpp
	languages-manager.cpp
	modules.cpp
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
	accounts/account-manager.h
	accounts/account-shared.h

	accounts/filter/abstract-account-filter.h
	accounts/filter/have-multilogon-filter.h
	accounts/filter/id-validity-filter.h
	accounts/filter/protocol-filter.h
	accounts/filter/writeable-contacts-list-filter.h

	accounts/model/accounts-model.h
	accounts/model/accounts-proxy-model.h

	avatars/avatar-job-manager.h
	avatars/avatar-job-runner.h
	avatars/avatar-manager.h
	avatars/avatar-shared.h

	buddies/buddy-additional-data-delete-handler-manager.h
	buddies/buddy-kadu-data.h
	buddies/buddy-shared.h
	buddies/buddy-manager.h
	buddies/group-manager.h
	buddies/buddy-preferred-manager.h
	buddies/group-shared.h

	buddies/filter/abstract-buddy-filter.h
	buddies/filter/account-buddy-filter.h
	buddies/filter/anonymous-buddy-filter.h
	buddies/filter/anonymous-without-messages-buddy-filter.h
	buddies/filter/blocked-buddy-filter.h
	buddies/filter/buddy-name-filter.h
	buddies/filter/group-buddy-filter.h
	buddies/filter/has-description-buddy-filter.h
	buddies/filter/mobile-buddy-filter.h
	buddies/filter/non-buddy-filter.h
	buddies/filter/offline-buddy-filter.h
	buddies/filter/online-and-description-buddy-filter.h
	buddies/filter/pending-messages-filter.h

	buddies/model/buddy-list-model.h
	buddies/model/buddies-model.h
	buddies/model/buddies-model-base.h
	buddies/model/buddies-model-proxy.h
	buddies/model/groups-model.h

	chat/chat-details.h
	chat/chat-details-aggregate.h
	chat/chat-details-conference.h
	chat/chat-details-simple.h
	chat/chat-geometry-data.h

	chat/aggregate-chat-manager.h
	chat/chat-manager.h
	chat/chat-shared.h
	chat/chat-styles-manager.h
	chat/html-messages-renderer.h
	chat/recent-chat-manager.h

	chat/filter/chat-filter.h
	chat/filter/chat-name-filter.h

	chat/message/message-render-info.h
	chat/message/pending-messages-manager.h
	chat/message/message-shared.h

	chat/style-engines/chat-engine-adium/chat-engine-adium.h
	chat/style-engines/chat-engine-kadu/chat-engine-kadu.h

	chat/type/chat-type.h
	chat/type/chat-type-conference.h
	chat/type/chat-type-manager.h
	chat/type/chat-type-simple.h

	contacts/contact-manager.h
	contacts/contact-shared.h

	contacts/filter/abstract-contact-filter.h
	contacts/filter/contact-no-unloaded-account-filter.h

	contacts/model/buddy-contact-model.h
	contacts/model/contact-list-model.h

	configuration/config-file-data-manager.h
	configuration/configuration-window-data-manager.h
	configuration/main-configuration.h
	configuration/notifier-configuration-data-manager.h
	configuration/toolbar-configuration-manager.h

	core/core.h

	emoticons/emoticon-selector-button.h
	emoticons/emoticon-selector.h
	emoticons/emoticons-manager.h

	gui/hot-key.h

	gui/actions/action.h
	gui/actions/actions.h
	gui/actions/action-description.h

	gui/widgets/account-add-widget.h
	gui/widgets/account-avatar-widget.h
	gui/widgets/account-buddy-list-widget.h
	gui/widgets/account-create-widget.h
	gui/widgets/account-edit-widget.h
	gui/widgets/accounts-combo-box.h
	gui/widgets/buddies-list-view.h
	gui/widgets/buddies-list-view-delegate.h
	gui/widgets/buddies-list-widget.h
	gui/widgets/buddy-avatar-widget.h
	gui/widgets/buddy-contacts-table-delegate.h
	gui/widgets/buddy-contacts-table-item.h
	gui/widgets/buddy-contacts-table-model.h
	gui/widgets/buddy-contacts-table-model-proxy.h
	gui/widgets/buddy-contacts-table.h
	gui/widgets/buddy-general-configuration-widget.h
	gui/widgets/buddy-groups-configuration-widget.h
	gui/widgets/buddies-list-view-menu-manager.h
	gui/widgets/buddy-personal-info-configuration-widget.h
	gui/widgets/buddy-options-configuration-widget.h
	gui/widgets/buddy-info-panel.h
	gui/widgets/chat-edit-box.h
	gui/widgets/chat-edit-box-size-manager.h
	gui/widgets/chat-messages-view.h
	gui/widgets/chat-view-network-access-manager.h
	gui/widgets/chat-widget.h
	gui/widgets/chat-widget-actions.h
	gui/widgets/chat-widget-manager.h
	gui/widgets/choose-identity-widget.h
	gui/widgets/color-button.h
	gui/widgets/color-selector.h
	gui/widgets/custom-input.h
	gui/widgets/delayed-line-edit.h
	gui/widgets/filter-widget.h
	gui/widgets/group-tab-bar.h
	gui/widgets/groups-combo-box.h
	gui/widgets/identities-combo-box.h
	gui/widgets/kadu-web-view.h
	gui/widgets/line-edit-clear-button.h
	gui/widgets/line-edit-with-clear-button.h
	gui/widgets/modal-configuration-widget.h
	gui/widgets/path-edit.h
	gui/widgets/path-list-edit.h
	gui/widgets/preview.h
	gui/widgets/progress-icon.h
	gui/widgets/progress-label.h
	gui/widgets/protocols-combo-box.h
	gui/widgets/proxy-group-box.h
	gui/widgets/select-buddy-combo-box.h
	gui/widgets/select-buddy-popup.h
	gui/widgets/select-file.h
	gui/widgets/select-font.h
	gui/widgets/status-button.h
	gui/widgets/status-buttons.h
	gui/widgets/status-menu.h
	gui/widgets/syntax-editor.h
	gui/widgets/toolbar.h

	gui/widgets/configuration/buddy-list-background-colors-widget.h
	gui/widgets/configuration/config-action-button.h
	gui/widgets/configuration/config-check-box.h
	#gui/widgets/configuration/config-widget-value.h
	gui/widgets/configuration/configuration-widget.h
	gui/widgets/configuration/config-label.h
	gui/widgets/configuration/config-color-button.h
	gui/widgets/configuration/config-combo-box.h
	gui/widgets/configuration/config-gg-password-edit.h
	gui/widgets/configuration/config-group-box.h
	gui/widgets/configuration/config-hot-key-edit.h
	gui/widgets/configuration/config-line-edit.h
	gui/widgets/configuration/config-line-separator.h
	gui/widgets/configuration/config-list-widget.h
	gui/widgets/configuration/config-path-list-edit.h
	gui/widgets/configuration/config-preview.h
	gui/widgets/configuration/config-radio-button.h
	gui/widgets/configuration/config-section.h
	gui/widgets/configuration/config-select-file.h
	gui/widgets/configuration/config-select-font.h
	gui/widgets/configuration/config-slider.h
	gui/widgets/configuration/config-spin-box.h
	gui/widgets/configuration/config-syntax-editor.h
	gui/widgets/configuration/config-tab.h
	gui/widgets/configuration/kadu-scroll-area.h
	gui/widgets/configuration/kadu-tab-widget.h
	gui/widgets/configuration/notify-tree-widget.h
	gui/widgets/configuration/notifier-configuration-widget.h
	gui/widgets/configuration/notify-group-box.h

	gui/windows/about.h
	gui/windows/add-buddy-window.h
	gui/windows/buddy-data-window.h
	gui/windows/buddy-delete-window.h
	gui/windows/chat-window.h
	gui/windows/choose-description.h
	gui/windows/configuration-window.h
	gui/windows/group-properties-window.h
	gui/windows/kadu-window.h
	gui/windows/kadu-window-actions.h
	gui/windows/main-configuration-window.h
	gui/windows/main-window.h
	gui/windows/merge-buddies-window.h
	gui/windows/modules-window.h
	gui/windows/multilogon-window.h
	gui/windows/password-window.h
	gui/windows/progress-window.h
	gui/windows/search-window.h
	gui/windows/search-window-actions.h
	gui/windows/subscription-window.h
	gui/windows/syntax-editor-window.h
	gui/windows/updates-dialog.h
	gui/windows/window-notifier-window.h
	gui/windows/your-accounts.h

	gui/windows/open-chat-with/open-chat-with.h
	gui/windows/open-chat-with/open-chat-with-runner-manager.h

	identities/model/identity-model.h
	identities/identity-manager.h
	identities/identity-shared.h

	misc/syntax-list.h

	model/actions-proxy-model.h

	multilogon/model/multilogon-model.h

	multilogon/multilogon-session.h

	notify/account-notification.h
	notify/buddy-notify-data.h
	notify/chat-notification.h
	notify/new-message-notification.h
	notify/multilogon-notification.h
	notify/notification.h
	notify/notification-manager.h
	notify/notifier.h
	notify/notify-configuration-ui-handler.h
	notify/status-changed-notification.h
	notify/window-notifier.h

	os/qtsingleapplication/qtlocalpeer.h

	protocols/connection-error-notification.h
	protocols/protocol.h
	protocols/protocol-factory.h
	protocols/protocols-manager.h
	protocols/filter/abstract-protocol-filter.h
	protocols/filter/can-register-protocol-filter.h
	protocols/model/protocols-model.h
	protocols/model/protocols-model-proxy.h
	protocols/services/avatar-service.h
	protocols/services/chat-image-service.h
	protocols/services/chat-service.h
	protocols/services/chat-state-service.h
	protocols/services/contact-list-service.h
	protocols/services/contact-personal-info-service.h
	protocols/services/multilogon-service.h
	protocols/services/personal-info-service.h
	protocols/services/roster-service.h
	protocols/services/search-service.h

	qt/long-validator.h

	status/base-status-container.h
	status/description-manager.h
	status/description-model.h
	status/status-actions.h
	status/status-changer.h
	status/status-changer-manager.h
	status/status-container.h
	status/status-container-manager.h

	storage/module-data.h

	themes/emoticon-theme-manager.h
	themes/icon-theme-manager.h
	themes/theme-manager.h

	dnshandler.h
	icons-manager.h
	kadu-application.h
	modules.h
	themes.h
	updates.h
)

if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_FOUND)
		message (STATUS "Found X11 libraries: ${X11_X11_LIB}")
	else (X11_Xfixes_FOUND)
		message (FATAL_ERROR "Could not find X11 libraries")
	endif (X11_FOUND)

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

qt_wrap_cpp (kadu MOC_FILES ${kadu_MOC_SRCS})

link_directories (modules)

if (WIN32)
	message (STATUS "Copying headers")
	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/copy_headers.bat
		${CMAKE_CURRENT_SOURCE_DIR}
		ARGS OFF
		OUTPUT_VARIABLE NULL_VAR
	)

	set (MOC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}${CMAKE_FILES_DIRECTORY}/kadu_core.dir)
	set (FINAL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/final.cpp)

	exec_program (echo ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "/* AUTOGENERATED */ > ${FINAL_SRC}"
		OUTPUT_VARIABLE NULL_VAR
	)
	message (STATUS "Final source cleaned: ${FINAL_SRC}")

	message (STATUS "Generating final sources")
	foreach (SRC ${kadu_SRCS})
		exec_program (${CMAKE_CURRENT_SOURCE_DIR}/merge_files.bat
			${CMAKE_CURRENT_SOURCE_DIR}
			ARGS "${SRC}" "${FINAL_SRC}"
			OUTPUT_VARIABLE NULL_VAR
		)
	endforeach (SRC)
	message (STATUS "Sources generated")

	set (FINAL_SRC
		${FINAL_SRC}
		os/qtsingleapplication/qtlocalpeer.cpp
		os/qtsingleapplication/qtlockedfile.cpp
		os/qtsingleapplication/qtlockedfile_win.cpp
	)

	add_library (kadu_core SHARED ${FINAL_SRC} ${MOC_FILES} kadu_core.rc)
	set_target_properties (kadu_core PROPERTIES DEFINE_SYMBOL KADULIB)
	set_target_properties (kadu_core PROPERTIES COMPILE_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/includes")
	target_link_libraries (kadu_core ${STATIC_MODULES} ${QT_LIBRARIES})
	set_property (TARGET kadu_core PROPERTY LINK_INTERFACE_LIBRARIES "")

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

	set_target_properties (kadu_c PROPERTIES COMPILE_FLAGS "-DKADU_CONSOLE")

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
	set_target_properties (kadu PROPERTIES ENABLE_EXPORTS ON)
	set_target_properties (kadu PROPERTIES COMPILE_FLAGS "-DKADULIB")

	target_link_libraries (kadu ${STATIC_MODULES} ${QT_LIBRARIES} ${CMAKE_DL_LIBS})

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
	install (FILES ${DESKTOP_ENTRY} DESTINATION share/applications)

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
endif (APPLE)
