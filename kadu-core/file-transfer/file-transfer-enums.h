/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_ENUMS_H
#define FILE_TRANSFER_ENUMS_H

enum FileTransferType {
	TypeSend,
	TypeReceive
};

enum FileTransferStatus {
	StatusNotConnected,
	StatusWaitingForConnection,
	StatusWaitingForAccept,
	StatusTransfer,
	StatusFinished,
	StatusRejected
};

enum FileTransferError {
	ErrorOk,
	ErrorNetworkError,
	ErrorUnableToOpenFile
};

enum StartType {
	StartNew,
	StartRestore
};

#endif FILE_TRANSFER_ENUMS_H
