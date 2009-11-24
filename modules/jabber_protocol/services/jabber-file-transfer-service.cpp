/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "file-transfer/jabber-file-transfer.h"
#include "jabber-protocol.h"

#include "jabber-file-transfer-service.h"

JabberFileTransferService::JabberFileTransferService(JabberProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
}

FileTransfer * JabberFileTransferService::loadFileTransferFromStorage(StoragePoint *storage)
{
	JabberFileTransfer *jft = new JabberFileTransfer(Protocol->account());
	jft->setStorage(storage);
	jft->load();

	return jft;
}

FileTransfer * JabberFileTransferService::createOutgoingFileTransfer(Buddy buddy)
{
	return new JabberFileTransfer(Protocol->account(), buddy, FileTransfer::TypeSend);
}

void JabberFileTransferService::incomingFile(JabberFileTransfer *transfer)
{
		emit incomingFileTransfer(transfer);
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
