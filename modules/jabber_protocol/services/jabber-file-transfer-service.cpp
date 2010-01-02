/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "file-transfer/jabber-file-transfer-handler.h"
#include "jabber-protocol.h"

#include "jabber-file-transfer-service.h"

JabberFileTransferService::JabberFileTransferService(JabberProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
}

FileTransferHandler * JabberFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	JabberFileTransferHandler *handler = new JabberFileTransferHandler(fileTransfer);
	fileTransfer.setHandler(handler);

	return handler;
}

void JabberFileTransferService::incomingFile(FileTransfer transfer)
{
	emit incomingFileTransfer(transfer);
}
