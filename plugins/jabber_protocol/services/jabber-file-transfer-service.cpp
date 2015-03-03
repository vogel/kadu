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

#include "jabber-file-transfer-service.h"

#include "file-transfer/jabber-outgoing-file-transfer-handler.h"
#include "file-transfer/jabber-stream-incoming-file-transfer-handler.h"
#include "services/jabber-resource-service.h"
#include "jabber-account-details.h"
#include "jid.h"

#include "core/core.h"
#include "contacts/contact-manager.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/gui/file-transfer-can-send-result.h"

#include <qxmpp/QXmppTransferManager.h>

JabberFileTransferService::JabberFileTransferService(QXmppTransferManager *transferManager, Account account, QObject *parent) :
		FileTransferService{parent},
		m_transferManager{transferManager},
		m_account{account}
{
	auto details = dynamic_cast<JabberAccountDetails *>(account.details());
	connect(details, SIGNAL(dataTransferProxyChanged()), this, SLOT(dataTransferProxyChanged()));
	dataTransferProxyChanged();

	connect(m_transferManager, SIGNAL(fileReceived(QXmppTransferJob*)), this, SLOT(fileReceived(QXmppTransferJob*)));
}

JabberFileTransferService::~JabberFileTransferService()
{
}

void JabberFileTransferService::setResourceService(JabberResourceService *resourceService)
{
	m_resourceService = resourceService;
}

FileTransferHandler * JabberFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	switch (fileTransfer.transferDirection())
	{
		case FileTransferDirection::Incoming:
			return new JabberStreamIncomingFileTransferHandler{fileTransfer};
		case FileTransferDirection::Outgoing:
		{
			auto handler = new JabberOutgoingFileTransferHandler{m_transferManager, fileTransfer};
			handler->setResourceService(m_resourceService);
			return handler;
		}
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

void JabberFileTransferService::dataTransferProxyChanged()
{
	auto details = dynamic_cast<JabberAccountDetails *>(m_account.details());
	m_transferManager->setProxy(details->dataTransferProxy());
	m_transferManager->setProxyOnly(details->requireDataTransferProxy());
}

void JabberFileTransferService::fileReceived(QXmppTransferJob *transferJob)
{
	auto jid = Jid::parse(transferJob->jid());
	auto peer = ContactManager::instance()->byId(m_account, jid.bare(), ActionCreateAndAdd);

	auto transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferDirection(FileTransferDirection::Incoming);
	transfer.setTransferType(FileTransferType::Stream);
	transfer.setTransferStatus(FileTransferStatus::WaitingForAccept);
	transfer.setRemoteFileName(transferJob->fileName());
	transfer.setFileSize(transferJob->fileSize());

	if (!Core::instance()->fileTransferHandlerManager()->ensureHandler(transfer))
		return;

	auto handler = qobject_cast<JabberStreamIncomingFileTransferHandler *>(transfer.handler());
	if (handler)
		handler->setTransferJob(transferJob);

	emit incomingFileTransfer(transfer);
}

#include "moc_jabber-file-transfer-service.cpp"
