/*
 * %kadu copyright begin%
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

#pragma once

#include "protocols/services/file-transfer-service.h"

#include "accounts/account.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ContactManager;
class FileTransferHandlerManager;
class FileTransferStorage;
class JabberResourceService;
class Myself;

class QXmppTransferJob;
class QXmppTransferManager;

class JabberFileTransferService : public FileTransferService
{
	Q_OBJECT

public:
	explicit JabberFileTransferService(QXmppTransferManager *transferManager, Account account, QObject *parent = nullptr);
	virtual ~JabberFileTransferService();

	void setResourceService(JabberResourceService *resourceService);

	virtual FileTransferHandler * createFileTransferHandler(FileTransfer fileTransfer) override;
	virtual FileTransferCanSendResult canSend(Contact contact) override;

private:
	QPointer<ContactManager> m_contactManager;
	QPointer<FileTransferHandlerManager> m_fileTransferHandlerManager;
	QPointer<FileTransferStorage> m_fileTransferStorage;
	QPointer<JabberResourceService> m_resourceService;
	QPointer<Myself> m_myself;

	QPointer<QXmppTransferManager> m_transferManager;
	Account m_account;

private slots:
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setFileTransferHandlerManager(FileTransferHandlerManager *fileTransferHandlerManager);
	INJEQT_SET void setFileTransferStorage(FileTransferStorage *fileTransferStorage);
	INJEQT_SET void setMyself(Myself *myself);

	void dataTransferProxyChanged();
	void fileReceived(QXmppTransferJob *transferJob);

};
