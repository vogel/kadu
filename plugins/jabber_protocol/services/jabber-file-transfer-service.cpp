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
#include "jabber-account-data.h"
#include "jid.h"
#include "services/jabber-resource-service.h"

#include "contacts/contact-manager.h"
#include "core/myself.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-storage.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/gui/file-transfer-can-send-result.h"

#include <qxmpp/QXmppTransferManager.h>

JabberFileTransferService::JabberFileTransferService(
    QXmppTransferManager *transferManager, Account account, QObject *parent)
        : FileTransferService{parent}, m_transferManager{transferManager}, m_account{account}
{
    connect(account, SIGNAL(updated()), this, SLOT(accountUpdated()));
    accountUpdated();

    connect(m_transferManager, SIGNAL(fileReceived(QXmppTransferJob *)), this, SLOT(fileReceived(QXmppTransferJob *)));
}

JabberFileTransferService::~JabberFileTransferService()
{
}

void JabberFileTransferService::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void JabberFileTransferService::setFileTransferHandlerManager(FileTransferHandlerManager *fileTransferHandlerManager)
{
    m_fileTransferHandlerManager = fileTransferHandlerManager;
}

void JabberFileTransferService::setFileTransferStorage(FileTransferStorage *fileTransferStorage)
{
    m_fileTransferStorage = fileTransferStorage;
}

void JabberFileTransferService::setMyself(Myself *myself)
{
    m_myself = myself;
}

void JabberFileTransferService::setResourceService(JabberResourceService *resourceService)
{
    m_resourceService = resourceService;
}

FileTransferHandler *JabberFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
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
    if (m_myself->buddy() == contact.ownerBuddy())
        return {false, {}};

    return {true, {}};
}

void JabberFileTransferService::accountUpdated()
{
    auto accountData = JabberAccountData{m_account};
    m_transferManager->setProxy(accountData.dataTransferProxy());
    m_transferManager->setProxyOnly(accountData.requireDataTransferProxy());
}

void JabberFileTransferService::fileReceived(QXmppTransferJob *transferJob)
{
    auto jid = Jid::parse(transferJob->jid());
    auto peer = m_contactManager->byId(m_account, jid.bare(), ActionCreateAndAdd);

    auto transfer = m_fileTransferStorage->create();
    transfer.setPeer(peer);
    transfer.setTransferDirection(FileTransferDirection::Incoming);
    transfer.setTransferType(FileTransferType::Stream);
    transfer.setTransferStatus(FileTransferStatus::WaitingForAccept);
    transfer.setRemoteFileName(transferJob->fileName());
    transfer.setFileSize(transferJob->fileSize());

    if (!m_fileTransferHandlerManager->ensureHandler(transfer))
        return;

    auto handler = qobject_cast<JabberStreamIncomingFileTransferHandler *>(transfer.handler());
    if (handler)
        handler->setTransferJob(transferJob);

    emit incomingFileTransfer(transfer);
}

#include "moc_jabber-file-transfer-service.cpp"
