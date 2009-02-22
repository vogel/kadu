/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"

#include "file-transfer/gadu-file-transfer.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer-service.h"

GaduFileTransferService::GaduFileTransferService(GaduProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
}

FileTransfer * GaduFileTransferService::createOutgoingFileTransfer(Contact contact)
{
	GaduFileTransfer *gft = new GaduFileTransfer(Protocol, contact, FileTransfer::TypeSend);
	return gft;
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
