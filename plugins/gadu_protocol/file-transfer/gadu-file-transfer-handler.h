/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#pragma once

#include "file-transfer/file-transfer-handler.h"

#include "services/drive/gadu-drive-send-ticket.h"

#include <QtCore/QPointer>

class GaduDriveGetTransfer;
class GaduDrivePutTransfer;
class GaduProtocol;

class QNetworkReply;

class GaduFileTransferHandler : public FileTransferHandler
{
	Q_OBJECT

	void finished(bool ok);

public:
	explicit GaduFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer);
	virtual ~GaduFileTransferHandler();

	virtual void send(QIODevice *source);
	virtual void stop();

	virtual void accept(QIODevice *destination);
	virtual void reject();

private:
	QPointer<GaduProtocol> m_protocol;
	GaduDriveSendTicket m_ticket;
	QPointer<GaduDriveGetTransfer> m_getTransfer;
	QPointer<GaduDrivePutTransfer> m_putTransfer;
	QPointer<QIODevice> m_source;
	bool m_putStarted;

	void startOutgoingTransferIfNotStarted();
	void updateStatus();

private slots:
	void statusUpdateReceived(GaduDriveSendTicket);
	void requestSendStatusUpdate();
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFinished(QNetworkReply*);

};
