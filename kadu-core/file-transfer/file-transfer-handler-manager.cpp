/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "file-transfer-handler-manager.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"

FileTransferHandlerManager::FileTransferHandlerManager(QObject *parent) :
		QObject{parent}
{
}

FileTransferHandlerManager::~FileTransferHandlerManager()
{
}

void FileTransferHandlerManager::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void FileTransferHandlerManager::setFileTransferManager(FileTransferManager *fileTransferManager)
{
	m_fileTransferManager = fileTransferManager;
}

void FileTransferHandlerManager::init()
{
	connect(m_fileTransferManager.data(), SIGNAL(fileTransferAboutToBeAdded(FileTransfer)), this, SLOT(fileTransferAboutToBeAdded(FileTransfer)));
	connect(m_fileTransferManager.data(), SIGNAL(fileTransferAboutToBeRemoved(FileTransfer)), this, SLOT(fileTransferRemoved(FileTransfer)));

	triggerAllAccountsAdded(m_accountManager);
}

void FileTransferHandlerManager::done()
{
	triggerAllAccountsRemoved(m_accountManager);
}

void FileTransferHandlerManager::createHandlers(Account account)
{
	for (auto &&transfer : m_fileTransferManager->items())
		if (transfer.peer().contactAccount() == account)
			createHandler(transfer);
}

void FileTransferHandlerManager::removeHandlers(Account account)
{
	for (auto &&transfer : m_fileTransferManager->items())
		if (transfer.peer().contactAccount() == account)
			removeHandler(transfer);
}

void FileTransferHandlerManager::accountAdded(Account account)
{
	connect(account, SIGNAL(protocolHandlerChanged()), this, SLOT(protocolHandlerChanged()));
	protocolHandlerChanged(account);
}

void FileTransferHandlerManager::accountRemoved(Account account)
{
	disconnect(account, SIGNAL(protocolHandlerChanged()), this, SLOT(protocolHandlerChanged()));
	protocolHandlerChanged(account);
}

void FileTransferHandlerManager::protocolHandlerChanged()
{
	auto account = Account{sender()};
	if (account)
		protocolHandlerChanged(account);
}

void FileTransferHandlerManager::protocolHandlerChanged(Account account)
{
	if (account.protocolHandler())
		createHandlers(account);
	else
		removeHandlers(account);
}

bool FileTransferHandlerManager::ensureHandler(FileTransfer transfer)
{
	createHandler(transfer);
	return transfer.handler() != nullptr;
}

void FileTransferHandlerManager::createHandler(FileTransfer transfer)
{
	if (!transfer || transfer.handler())
		return;

	auto protocol = transfer.peer().contactAccount().protocolHandler();
	if (!protocol)
		return;

	auto service = protocol->fileTransferService();
	if (!service)
		return;

	transfer.setHandler(service->createFileTransferHandler(transfer));
}

void FileTransferHandlerManager::removeHandler(FileTransfer transfer)
{
	if (!transfer || !transfer.handler())
		return;

	transfer.handler()->deleteLater();
	transfer.setHandler(nullptr);
}

void FileTransferHandlerManager::fileTransferAboutToBeAdded(FileTransfer fileTransfer)
{
	createHandler(fileTransfer);
}

void FileTransferHandlerManager::fileTransferRemoved(FileTransfer fileTransfer)
{
	removeHandler(fileTransfer);
}

#include "moc_file-transfer-handler-manager.cpp"
