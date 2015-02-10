set (file-transfer_SRCS
	gui/file-transfer-actions.cpp
	gui/file-transfer-can-send-result.cpp
	gui/file-transfer-widget.cpp
	gui/file-transfer-window.cpp
	gui/send-file-action.cpp
	gui/show-file-transfer-window-action.cpp

	file-transfer.cpp
	file-transfer-handler-manager.cpp
	file-transfer-handler.cpp
	file-transfer-manager.cpp
	file-transfer-module.cpp
	file-transfer-notifications.cpp
	file-transfer-shared.cpp
	outgoing-file-transfer-handler.cpp
	stream-incoming-file-transfer-handler.cpp
	url-incoming-file-transfer-handler.cpp
)

kadu_subdirectory (file-transfer "${file-transfer_SRCS}" "" "")
