set (file-transfer_SRCS
	file-transfer.cpp
	file-transfer-actions.cpp
	file-transfer-manager.cpp
	file-transfer-notifications.cpp
	file-transfer-shared.cpp
)

set (file-transfer_MOC_SRCS
	file-transfer-actions.h
	file-transfer-handler.h
	file-transfer-manager.h
	file-transfer-notifications.h
	file-transfer-shared.h
)

kadu_subdirectory (file-transfer "${file-transfer_SRCS}" "${file-transfer_MOC_SRCS}" "")
