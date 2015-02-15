/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "core/core.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/gui/file-transfer-can-send-result.h"

#include "file-transfer/jabber-outgoing-file-transfer-handler.h"
#include "file-transfer/jabber-stream-incoming-file-transfer-handler.h"
#include "file-transfer/s5b-server-manager.h"
#include "services/jabber-connection-service.h"
#include "jabber-protocol.h"

#include "jabber-file-transfer-service.h"

JabberFileTransferService::JabberFileTransferService(XMPP::JabberProtocol *protocol) :
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
	switch (fileTransfer.transferDirection())
	{
		case FileTransferDirection::Incoming:
			return new JabberStreamIncomingFileTransferHandler{fileTransfer};
		case FileTransferDirection::Outgoing:
			return new JabberOutgoingFileTransferHandler{fileTransfer};
		default:
			return nullptr;
	}
}

FileTransferCanSendResult JabberFileTransferService::canSend(Contact contact)
{
	if (Core::instance()->myself() == contact.ownerBuddy())
		return {false, {}};

	return {true, {}};
}

void JabberFileTransferService::loggedIn()
{
	S5BServerManager::instance()->addAddress(Protocol->connectionService()->localAddress());
	Protocol->xmppClient()->s5bManager()->setServer(S5BServerManager::instance()->server());
}

void JabberFileTransferService::loggedOut()
{
	S5BServerManager::instance()->removeAddress(Protocol->connectionService()->localAddress());
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
	transfer.setTransferDirection(FileTransferDirection::Incoming);
	transfer.setTransferType(FileTransferType::Stream);
	transfer.setTransferStatus(FileTransferStatus::WaitingForAccept);
	transfer.setRemoteFileName(jTransfer->fileName());
	transfer.setFileSize(jTransfer->fileSize());

	if (!Core::instance()->fileTransferHandlerManager()->ensureHandler(transfer))
		return;

	auto handler = qobject_cast<JabberStreamIncomingFileTransferHandler *>(transfer.handler());
	if (handler)
		handler->setJTransfer(jTransfer);

	emit incomingFileTransfer(transfer);
}

#include "moc_jabber-file-transfer-service.cpp"
