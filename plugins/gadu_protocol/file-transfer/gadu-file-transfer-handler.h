/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef GADU_FILE_TRANSFER_H
#define GADU_FILE_TRANSFER_H

#include "file-transfer/file-transfer-handler.h"

class DccSocketNotifiers;
class GaduProtocol;

class GaduFileTransferHandler : public FileTransferHandler
{
	Q_OBJECT

	friend class DccManager;
	friend class DccSocketNotifiers;
	DccSocketNotifiers *SocketNotifiers;
	bool WaitingForSocketNotifiers;

	void finished(bool ok);

private slots:
	void socketNotifiersDeleted();

protected:
	virtual void updateFileInfo();

public:
	GaduFileTransferHandler(FileTransfer fileTransfer);
	virtual ~GaduFileTransferHandler();

	void setFileTransferNotifiers(DccSocketNotifiers *);
	void socketNotAvailable();

	virtual void send();
	virtual void stop();

	virtual bool accept(const QString &fileName, bool resumeTransfer);
	virtual void reject();

};

#endif // GADU_FILE_TRANSFER_H
