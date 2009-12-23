/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "dcc/dcc-manager.h"
#include "file-transfer/gadu-file-transfer-handler.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer-service.h"

GaduFileTransferService::GaduFileTransferService(GaduProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
}

FileTransferHandler * GaduFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	GaduFileTransferHandler *handler = new GaduFileTransferHandler(fileTransfer);
	fileTransfer.setHandler(handler);

	return handler;
}
