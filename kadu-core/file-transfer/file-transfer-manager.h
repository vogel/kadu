/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#ifndef FILE_TRANSFER_MANAGER_H
#define FILE_TRANSFER_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/accounts-aware-object.h"
#include "file-transfer/file-transfer-enums.h"
#include "file-transfer/file-transfer.h"
#include "storage/simple-manager.h"

#include "exports.h"

class FileTransferActions;
class FileTransferWindow;
class XmlConfigFile;

class KADUAPI FileTransferManager : public QObject, public SimpleManager<FileTransfer>, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferManager)

	static FileTransferManager * Instance;

	FileTransferActions *Actions;
	FileTransferWindow *Window;

	FileTransferManager();
	virtual ~FileTransferManager();

	FileTransfer byPeerAndRemoteFileName(Contact peer, const QString &remoteFileName);

	void addFileTransferService(Account account);
	void removeFileTransferService(Account account);

private slots:
	void fileTransferWindowDestroyed();
	void incomingFileTransfer(FileTransfer fileTransfer);

	void fileTransferServiceRegistered();
	void fileTransferServiceUnregistered();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual void itemAboutToBeAdded(FileTransfer fileTransfer);
	virtual void itemAdded(FileTransfer fileTransfer);
	virtual void itemAboutToBeRemoved(FileTransfer fileTransfer);
	virtual void itemRemoved(FileTransfer fileTransfer);

public:
	static FileTransferManager * instance();

	virtual QString storageNodeName() { return QLatin1String("FileTransfersNew"); }
	virtual QString storageNodeItemName() { return QLatin1String("FileTransfer"); }

	void acceptFileTransfer(FileTransfer transfer);
	void rejectFileTransfer(FileTransfer transfer);

	void showFileTransferWindow();

	void cleanUp();

signals:
	void fileTransferAboutToBeAdded(FileTransfer fileTransfer);
	void fileTransferAdded(FileTransfer fileTransfer);
	void fileTransferAboutToBeRemoved(FileTransfer fileTransfer);
	void fileTransferRemoved(FileTransfer fileTransfer);

};

#endif // FILE_TRANSFER_MANAGER_H
