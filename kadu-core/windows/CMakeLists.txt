set (windows_SRCS
	chat-window/chat-window.cpp
	chat-window/chat-window-manager.cpp
	chat-window/chat-window-module.cpp
	chat-window/chat-window-repository.cpp
	chat-window/chat-window-storage.cpp
	chat-window/chat-window-storage-configuration.cpp
	chat-window/chat-window-storage-configurator.cpp
	chat-window/window-chat-widget-container-handler.cpp

	about.cpp
	add-buddy-window.cpp
	add-conference-window.cpp
	add-room-chat-window.cpp
	buddy-data-window.cpp
	buddy-data-window-repository.cpp
	buddy-delete-window.cpp
	chat-data-window.cpp
	chat-data-window-repository.cpp
	configuration-window.cpp
	group-edit-window.cpp
	kadu-dialog.cpp
	kadu-window-service.cpp
	kadu-window.cpp
	kadu-window-actions.cpp
	main-configuration-window-service.cpp
	main-configuration-window.cpp
	main-window-repository.cpp
	main-window.cpp
	message-dialog.cpp
	multilogon-window-service.cpp
	multilogon-window.cpp
	progress-window.cpp
	proxy-action-context.cpp
	proxy-edit-window-service.cpp
	proxy-edit-window.cpp
	search-window.cpp
	status-window-description-proxy-model.cpp
	status-window-service.cpp
	status-window.cpp
	string-list-dialog.cpp
	subscription-window.cpp
	token-window.cpp
	updates-dialog.cpp
	window-manager.cpp
	your-accounts-window-service.cpp
	your-accounts.cpp

	open-chat-with/open-chat-with-contact-list-runner.cpp
	open-chat-with/open-chat-with-runner-manager.cpp
	open-chat-with/open-chat-with-service.cpp
	open-chat-with/open-chat-with.cpp
)

if (MINGW)
	set_property (SOURCE open-chat-with/open-chat-with.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -fno-strict-aliasing")
endif ()

kadu_subdirectory (windows "${windows_SRCS}" "" "")
