/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef FILE_TRANSFER_NOTIFICATION_H
#define FILE_TRANSFER_NOTIFICATION_H

#include "file-transfer/file-transfer.h"
#include "notify/chat-notification.h"

class FileTransferManager;
class DccSocket;
class NotifyEvent;

class NewFileTransferNotification : public ChatNotification
{
	Q_OBJECT

	static NotifyEvent *FileTransferNotifyEvent;
	static NotifyEvent *FileTransferIncomingFileNotifyEvent;

	bool Continue;

	FileTransfer ft;

	QString fileName;

private slots:
	void callbackAcceptAsNew();

public slots:
	virtual void callbackAccept();
	virtual void callbackReject();

public:
	static void registerEvents();
	static void unregisterEvents();

	NewFileTransferNotification(const QString &type, FileTransfer ft, Chat chat, StartType startType);

	virtual bool requireCallback() { return true; }

};

#endif // FILE_TRANSFER_NOTIFICATION_H
