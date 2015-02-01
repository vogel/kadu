set (file-transfer_SRCS
	file-transfer.cpp
	file-transfer-actions.cpp
	file-transfer-handler.cpp
	file-transfer-manager.cpp
	file-transfer-notifications.cpp
	file-transfer-shared.cpp
)

kadu_subdirectory (file-transfer "${file-transfer_SRCS}" "" "")
