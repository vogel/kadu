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

#pragma once

#include "accounts/accounts-aware-object.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class FileTransfer;
class FileTransferManager;

class KADUAPI FileTransferHandlerManager : public QObject, AccountsAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FileTransferHandlerManager(QObject *parent = nullptr);
	virtual ~FileTransferHandlerManager();

	bool ensureHandler(FileTransfer transfer);

private slots:
	INJEQT_SETTER void setFileTransferManager(FileTransferManager *fileTransferManager);

	void fileTransferAboutToBeAdded(FileTransfer fileTransfer);
	void fileTransferRemoved(FileTransfer fileTransfer);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

private:
	QPointer<FileTransferManager> m_fileTransferManager;

	void createHandlers(Account account);
	void removeHandlers(Account account);
	void createHandler(FileTransfer transfer);
	void removeHandler(FileTransfer transfer);

private slots:
	void protocolHandlerChanged();

};
