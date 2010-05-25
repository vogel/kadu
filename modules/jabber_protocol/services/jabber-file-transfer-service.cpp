/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include <iris/filetransfer.h>

#include "contacts/contact-manager.h"

#include "file-transfer/jabber-file-transfer-handler.h"
#include "jabber-protocol.h"

#include "jabber-file-transfer-service.h"

JabberFileTransferService::JabberFileTransferService(JabberProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
	connect(Protocol->client(), SIGNAL(incomingFileTransfer()),
			this, SLOT(irisIncomingFileTransfer()));
}

JabberFileTransferService::~JabberFileTransferService()
{
}

FileTransferHandler * JabberFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	JabberFileTransferHandler *handler = new JabberFileTransferHandler(fileTransfer);
	fileTransfer.setHandler(handler);

	return handler;
}

void JabberFileTransferService::irisIncomingFileTransfer()
{
	XMPP::FileTransfer *jTransfer = Protocol->client()->fileTransferManager()->takeIncoming();
	if (!jTransfer)
		return;

	Contact peer = ContactManager::instance()->byId(Protocol->account(), jTransfer->peer().bare(), ActionCreateAndAdd);
	FileTransfer transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferType(TypeReceive);
	transfer.setRemoteFileName(jTransfer->fileName());

	transfer.createHandler();

	JabberFileTransferHandler *handler = dynamic_cast<JabberFileTransferHandler *>(transfer.handler());
	if (handler)
		handler->setJTransfer(jTransfer);

	emit incomingFileTransfer(transfer);
}
