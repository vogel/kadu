/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <iris/filetransfer.h>

#include "contacts/contact-manager.h"

#include "file-transfer/jabber-file-transfer-handler.h"
#include "file-transfer/s5b-server-manager.h"
#include "jabber-protocol.h"

#include "jabber-file-transfer-service.h"

JabberFileTransferService::JabberFileTransferService(JabberProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
	connect(Protocol, SIGNAL(stateMachineLoggedIn()), this, SLOT(loggedIn()));
	connect(Protocol, SIGNAL(stateMachineLoggedOut()), this, SLOT(loggedOut()));

	Protocol->xmppClient()->setFileTransferEnabled(true);
	Protocol->xmppClient()->fileTransferManager()->setDisabled(XMPP::S5BManager::ns(), false);

	connect(Protocol->xmppClient()->fileTransferManager(), SIGNAL(incomingReady()),
			this, SLOT(incomingFileTransferSlot()));
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

void JabberFileTransferService::loggedIn()
{
	S5BServerManager::instance()->addAddress(Protocol->client()->localAddress());
	Protocol->xmppClient()->s5bManager()->setServer(S5BServerManager::instance()->server());
}

void JabberFileTransferService::loggedOut()
{
	S5BServerManager::instance()->removeAddress(Protocol->client()->localAddress());
	Protocol->xmppClient()->s5bManager()->setServer(0);
}

void JabberFileTransferService::incomingFileTransferSlot()
{
	XMPP::FileTransfer *jTransfer = Protocol->xmppClient()->fileTransferManager()->takeIncoming();
	if (!jTransfer)
		return;

	Contact peer = ContactManager::instance()->byId(Protocol->account(), jTransfer->peer().bare(), ActionCreateAndAdd);
	FileTransfer transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferType(TypeReceive);
	transfer.setRemoteFileName(jTransfer->fileName());
	transfer.setFileSize(jTransfer->fileSize());

	transfer.createHandler();

	JabberFileTransferHandler *handler = dynamic_cast<JabberFileTransferHandler *>(transfer.handler());
	if (handler)
		handler->setJTransfer(jTransfer);

	emit incomingFileTransfer(transfer);
}
