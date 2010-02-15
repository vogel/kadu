/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

void GaduFileTransferService::newIncomingFileTransfer(FileTransfer fileTransfer)
{
	emit incomingFileTransfer(fileTransfer);
}
