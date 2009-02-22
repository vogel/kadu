/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "file-transfer.h"

FileTransfer::FileTransfer(Contact peer, FileTransferType transferType) :
		Peer(peer), TransferType(transferType), TransferStatus(StatusNotConnected)
{
}

FileTransfer::~FileTransfer()
{
}

void FileTransfer::changeFileTransferStatus(FileTransferStatus transferStatus)
{
	if (TransferStatus == transferStatus)
		return;

	TransferStatus = transferStatus;
	emit statusChanged(TransferStatus);
}
