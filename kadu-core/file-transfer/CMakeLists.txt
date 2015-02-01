set (file-transfer_SRCS
	gui/file-transfer-widget.cpp
	gui/file-transfer-window.cpp

	file-transfer.cpp
	file-transfer-actions.cpp
	file-transfer-handler.cpp
	file-transfer-manager.cpp
	file-transfer-notifications.cpp
	file-transfer-shared.cpp
)

kadu_subdirectory (file-transfer "${file-transfer_SRCS}" "" "")
