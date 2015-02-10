/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "accounts/accounts-aware-object.h"
#include "file-transfer/file-transfer.h"
#include "storage/simple-manager.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUuid>
#include <injeqt/injeqt.h>

class FileTransferActions;
class FileTransferHandlerManager;
class FileTransferWindow;
class ConfigurationApi;

class KADUAPI FileTransferManager : public QObject, public SimpleManager<FileTransfer>, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferManager)

public:
	Q_INVOKABLE explicit FileTransferManager(QObject *parent = nullptr);
	virtual ~FileTransferManager();

	virtual QString storageNodeName() { return QLatin1String("FileTransfersNew"); }
	virtual QString storageNodeItemName() { return QLatin1String("FileTransfer"); }

	void acceptFileTransfer(FileTransfer transfer);
	void rejectFileTransfer(FileTransfer transfer);
	void sendFile(FileTransfer transfer, QString fileName);
	QString getSaveFileName(QString localFileName, QString remoteFileName, QWidget *parent = nullptr);
	void showFileTransferWindow();
	void cleanUp();

signals:
	void fileTransferAboutToBeAdded(FileTransfer fileTransfer);
	void fileTransferAdded(FileTransfer fileTransfer);
	void fileTransferAboutToBeRemoved(FileTransfer fileTransfer);
	void fileTransferRemoved(FileTransfer fileTransfer);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void itemAboutToBeAdded(FileTransfer fileTransfer);
	virtual void itemAdded(FileTransfer fileTransfer);
	virtual void itemAboutToBeRemoved(FileTransfer fileTransfer);
	virtual void itemRemoved(FileTransfer fileTransfer);

private:
	FileTransferActions *m_actions;
	QPointer<FileTransferHandlerManager> m_fileTransferHandlerManager;
	QPointer<FileTransferWindow> m_window;

	void addFileTransferService(Account account);
	void removeFileTransferService(Account account);

private slots:
	INJEQT_SETTER void setFileTransferHandlerManager(FileTransferHandlerManager *fileTransferHandlerManager);

	void incomingFileTransfer(FileTransfer fileTransfer);

};
