/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FILE_TRANSFER_SERVICE_H
#define FILE_TRANSFER_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "file-transfer/file-transfer.h"

#include "exports.h"

class FileTransferCanSendResult;
class FileTransferHandler;

class KADUAPI FileTransferService : public QObject
{
	Q_OBJECT

public:
	explicit FileTransferService(QObject *parent) : QObject(parent) {}

	virtual FileTransferHandler * createFileTransferHandler(FileTransfer fileTransfer) = 0;
	virtual FileTransferCanSendResult canSend(Contact contact) = 0;

signals:
	void canSendChanged();
	void incomingFileTransfer(FileTransfer fileTransfer);

};

#endif // FILE_TRANSFER_SERVICE_H
