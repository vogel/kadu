/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "debug.h"

#include "file-transfer/gadu-file-transfer-handler.h"
#include "helpers/gadu-protocol-helper.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer-service.h"
#include <QUrl>

GaduFileTransferService::GaduFileTransferService(GaduProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
}

GaduFileTransferService::~GaduFileTransferService()
{
}

FileTransferHandler * GaduFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	auto handler = new GaduFileTransferHandler(Protocol, fileTransfer);
	fileTransfer.setHandler(handler);

	return handler;
}

void GaduFileTransferService::fileTransferReceived(Contact peer, QString downloadId, QString fileName)
{
	auto transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferType(TypeReceive);
	transfer.setRemoteFileName(QUrl::fromPercentEncoding(fileName.toUtf8()));
	transfer.setFileSize(0); // we don't know file size yet
	transfer.addProperty("gg:downloadId", downloadId, CustomProperties::Storable);
	transfer.addProperty("gg:remoteFileName", fileName, CustomProperties::Storable);
	transfer.createHandler();

	emit incomingFileTransfer(transfer);
}

#include "moc_gadu-file-transfer-service.cpp"
